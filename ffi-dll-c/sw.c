/*
Copyright (C) 2016

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "lal_matrix.h"
#include "lal_report.h"
#include "lal_debuglogs.h"

#include "sw.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SCORE(a, b, c, d) (((a) == (b)) ? (c) : (d))

#if defined(RANGE_DETECTION)
#define VDTABLE(a, b) (sp->mtx->sc_double_matrix.ddata[(a)][(b)])
#define VITABLE(a, b) (sp->mtx->sc_int_matrix.idata[(a)][(b)])
#else
static double vdtable(matrix_t *substitution, size_t a, size_t b) {
	if (a >= substitution->nrows && b >= substitution->ncols) {
		static_report_error("data range corruption: please, check input sequences for substitution matrix");
		return SCORE(a, b, 1.0, -1.0);
	}
	else {
		return substitution->ddata[a][b];
	}
}

static int64_t vitable(matrix_t *substitution, size_t a, size_t b) {
	if (a >= substitution->nrows && b >= substitution->ncols) {
		static_report_error("data range corruption: please, check input sequences for substitution matrix");
		return SCORE(a, b, 1, -1);
	}
	else {
		return substitution->idata[a][b];
	}
}
#define VDTABLE(a, b) (vdtable (&sp->mtx->sc_double_matrix, (a), (b)))
#define VITABLE(a, b) (vitable (&sp->mtx->sc_int_matrix, (a),(b)))
#endif
/*
 * Sequence alignments are used in different area of computer science.
 * Main feature of alignment is a gap function because complexity of
 * computation depends on this function. The most common and simple
 * case is using a constant gap penalties.
 */
double sw_constant_gap_double(const search_swcg_profile_t * sp, const sequence_t * dseq, const sequence_t * qseq)
{
	double d_last, u_last, l_last;
	double d_new, u_new, l_new;
	double v;
	matrix_t score_mat = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);
	for (size_t i = 0; i <= dseq->len; i++) {
		for (size_t j = 0; j <= qseq->len; j++) {
			// This is the first row / column which is all zeros
			if (i == 0 || j == 0) {
				score_mat.ddata[i][j] = 0;
				continue;
			}
			else {
				d_last = score_mat.ddata[i - 1][j - 1];
				u_last = score_mat.ddata[i - 1][j];
				l_last = score_mat.ddata[i][j - 1];
			}
			if (!sp->mtx)
				v = SCORE(dseq->seq[i - 1], qseq->seq[j - 1], 1.0, -1.0);
			else
				v = VDTABLE(dseq->seq[i - 1], qseq->seq[j - 1]);
			d_new = d_last + v;
			u_new = u_last + sp->gap;
			l_new = l_last + sp->gap;
			score_mat.ddata[i][j] = MAX(MAX(d_new, u_new), MAX(l_new, 0));
		}
	}

	element_t score = find_max(&score_mat);
	//	print_matrix(&score_mat);
	free_matrix(&score_mat);
	return score.d;
}

/*
* Sequence alignments are used in different area of computer science.
* Main feature of alignment is a gap function because complexity of
* computation depends on this function. The most common and simple
* case is using a constant gap penalties.
*/
int64_t sw_constant_gap_int(const search_swcg_profile_int_t * sp, const sequence_t * dseq, const sequence_t * qseq)
{
	int64_t d_last, u_last, l_last;
	int64_t d_new, u_new, l_new;
	int64_t v;
	matrix_t score_mat = matrix(dseq->len + 1, qseq->len + 1, INTTYPE);

	for (size_t i = 0; i <= dseq->len; i++) {
		for (size_t j = 0; j <= qseq->len; j++) {
			// This is the first row / column which is all zeros
			if (i == 0 || j == 0) {
				score_mat.idata[i][j] = 0;
				continue;
			}
			else {
				d_last = score_mat.idata[i - 1][j - 1];
				u_last = score_mat.idata[i - 1][j];
				l_last = score_mat.idata[i][j - 1];
			}
			if (!sp->mtx)
				v = SCORE(dseq->seq[i - 1], qseq->seq[j - 1], 1, -1);
			else
				v = VITABLE(dseq->seq[i - 1], qseq->seq[j - 1]);
			d_new = d_last + v;
			u_new = u_last + sp->gap;
			l_new = l_last + sp->gap;
			score_mat.idata[i][j] = MAX(MAX(d_new, u_new), MAX(l_new, 0));
		}
	}

	element_t score = find_max(&score_mat);
	//	print_matrix(&score_mat);
	free_matrix(&score_mat);
	return score.i;
}

/*
 * Sequence alignments with constant and linear gap penalties can be computed in time O(n*m) for two
 * sequences of lingth m and n. With affine gap penalties the time increases to O(n*m*(n+m)),
 * since for each cell the algorithm has to checkif a gap is extended or a new one is opened.
 * In 1982 Gotoh described a method to compute optimal sequence alignments, with affine gap
 * penalties, in time O(n*m). His version uses two additional matrices(E and F) to keep track of open gap.
 * E keeps track of gaps in the query sequence and F if gaps in the database sequence.
 * The values in E are calculated as the maximum of the previous value in H plus the costs Q for opening
 * a gap an the previous balue in E plus the costs T for extending a gap.  The values in F are
 * computed the same way, except for gaps in the other sequence. The values in H are computed like
 * in the Smith-Waterman with linear gap costs, except the value in E and F are used ad the gap costs.
 */
double sw_affine_gap(const search_swag_profile_t * sp, const sequence_t * dseq, const sequence_t * qseq)
{
	double d_last, u_last, l_last, e_last, f_last;
	double d_new, u_new, l_new, e_new, f_new;
	double v;
	matrix_t score_mat = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE); // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t ee = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);        // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t ff = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);        // todo: there is a bug. we have to use calloc instead of malloc 
	if (!score_mat.ddata) return 0.0;
	if (!ee.ddata) { free_matrix(&score_mat); return 0.0; }
	if (!ff.ddata) { free_matrix(&ee); return 0.0; }

	for (size_t i = 0; i <= dseq->len; i++) {
		for (size_t j = 0; j <= qseq->len; j++) {
			// This is the first row / column which is all zeros
			if (i == 0 || j == 0) {
				if (i == 0)
					ee.ddata[i][j] = 0;
				if (j == 0)
					ff.ddata[i][j] = 0;
				score_mat.ddata[i][j] = 0;
				continue;
			}
			else {
				e_last = ee.ddata[i][j - 1];
				f_last = ff.ddata[i - 1][j];
				d_last = score_mat.ddata[i - 1][j - 1];
				u_last = score_mat.ddata[i - 1][j];
				l_last = score_mat.ddata[i][j - 1];
			}
			if (!sp->mtx)
				v = SCORE(dseq->seq[i - 1], qseq->seq[j - 1], 1.0, -1.0);
			else
				v = VDTABLE(dseq->seq[i - 1], qseq->seq[j - 1]);
			d_new = d_last + v;
			u_new = u_last + sp->gapOpen;
			l_new = l_last + sp->gapOpen;
			e_new = e_last + sp->gapExt;
			f_new = f_last + sp->gapExt;
			ee.ddata[i][j] = MAX(e_new, l_new);
			ff.ddata[i][j] = MAX(f_new, u_new);
			score_mat.ddata[i][j] = MAX(MAX(d_new, ee.ddata[i][j]), MAX(ff.ddata[i][j], 0));
		}
	}

	element_t score = find_max(&score_mat);
	//	print_matrix(&score_mat);
	//	print_matrix(&ee);
	//	print_matrix(&ff);
	free_matrix(&score_mat);
	free_matrix(&ee);
	free_matrix(&ff);
	return score.d;
}


/*
* Sequence alignments with constant and linear gap penalties can be computed in time O(n*m) for two
* sequences of lingth m and n. With affine gap penalties the time increases to O(n*m*(n+m)),
* since for each cell the algorithm has to checkif a gap is extended or a new one is opened.
* In 1982 Gotoh described a method to compute optimal sequence alignments, with affine gap
* penalties, in time O(n*m). His version uses two additional matrices(E and F) to keep track of open gap.
* E keeps track of gaps in the query sequence and F if gaps in the database sequence.
* The values in E are calculated as the maximum of the previous value in H plus the costs Q for opening
* a gap an the previous balue in E plus the costs T for extending a gap.  The values in F are
* computed the same way, except for gaps in the other sequence. The values in H are computed like
* in the Smith-Waterman with linear gap costs, except the value in E and F are used ad the gap costs.
*/
double sw_affine_gap_fail(const search_swag_profile_t * sp, const sequence_t * dseq, const sequence_t * qseq)
{
	double v;
	matrix_t h = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE); // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t ee = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);        // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t ff = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);        // todo: there is a bug. we have to use calloc instead of malloc 
	if (!h.ddata) return 0.0;
	if (!ee.ddata) { free_matrix(&h); return 0.0; }
	if (!ff.ddata) { free_matrix(&ee); return 0.0; }

	double *hri0 = malloc((dseq->len + 1) * sizeof(double));
	double *fri0 = malloc((dseq->len + 1) * sizeof(double));

	for (size_t j = 0; j < dseq->len + 1; j++) {
		hri0[j] = 0;
		fri0[j] = 0;
	}

	double er = 0;
	for (size_t i = 0; i <= dseq->len; i++) {
		double hr = 0;
		for (size_t j = 0; j <= qseq->len; j++) {
			// This is the first row / column which is all zeros
			if (i == 0 || j == 0) {
				continue;
			}

			if (!sp->mtx)
				v = SCORE(dseq->seq[i - 1], qseq->seq[j - 1], 1.0, -1.0);
			else
				v = VDTABLE(dseq->seq[i - 1], qseq->seq[j - 1]);

			double mx_new = hri0[j] + sp->gapOpen;
			double my_new = hr + sp->gapOpen;
			hr = hri0[j] = h.ddata[i][j] = MAX(MAX(hri0[j - 1] + v, er + v), MAX(fri0[j - 1] + v, 0));
			er = ee.ddata[i][j] = MAX(er + sp->gapExt, my_new);
			fri0[j] = ff.ddata[i][j] = MAX(fri0[j] + sp->gapExt, mx_new);

		}
	}

	element_t score = find_max(&h);
	//	print_matrix(&h);
	//	print_matrix(&ee);
	//	print_matrix(&ff);
	free_matrix(&h);
	free_matrix(&ee);
	free_matrix(&ff);
	free(hri0);
	free(fri0);
	return score.d;
}

/*
* Sequence alignments with constant and linear gap penalties can be computed in time O(n*m) for two
* sequences of lingth m and n. With affine gap penalties the time increases to O(n*m*(n+m)),
* since for each cell the algorithm has to checkif a gap is extended or a new one is opened.
* In 1982 Gotoh described a method to compute optimal sequence alignments, with affine gap
* penalties, in time O(n*m). His version uses two additional matrices(E and F) to keep track of open gap.
* E keeps track of gaps in the query sequence and F if gaps in the database sequence.
* The values in E are calculated as the maximum of the previous value in H plus the costs Q for opening
* a gap an the previous balue in E plus the costs T for extending a gap.  The values in F are
* computed the same way, except for gaps in the other sequence. The values in H are computed like
* in the Smith-Waterman with linear gap costs, except the value in E and F are used ad the gap costs.
*/
double sw_affine_gap_sigaev2(const search_swag_profile_t * sp, const sequence_t * dseq, const sequence_t * qseq)
{
	double d_last, u_last, l_last, e_last, f_last;
	double d_new, u_new, l_new, e_new, f_new;
	double v;
	matrix_t score_mat = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE); // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t ee = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);        // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t ff = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);        // todo: there is a bug. we have to use calloc instead of malloc 
	if (!score_mat.ddata) return 0.0;
	if (!ee.ddata) { free_matrix(&score_mat); return 0.0; }
	if (!ff.ddata) { free_matrix(&ee); return 0.0; }

	for (size_t i = 0; i <= dseq->len; i++) {
		for (size_t j = 0; j <= qseq->len; j++) {
			// This is the first row / column which is all zeros
			if (i == 0 || j == 0) {
				if (i == 0)
					ee.ddata[i][j] = 0;
				if (j == 0)
					ff.ddata[i][j] = 0;
				score_mat.ddata[i][j] = 0;
				continue;
			}
			else {
				e_last = ee.ddata[i][j - 1];
				f_last = ff.ddata[i - 1][j];
				d_last = score_mat.ddata[i - 1][j - 1];
				u_last = score_mat.ddata[i - 1][j];
				l_last = score_mat.ddata[i][j - 1];
			}
			if (!sp->mtx)
				v = SCORE(dseq->seq[i - 1], qseq->seq[j - 1], 1.0, -1.0);
			else
				v = VDTABLE(dseq->seq[i - 1], qseq->seq[j - 1]);
			d_new = d_last + v;
			u_new = u_last + sp->gapOpen;
			l_new = l_last + sp->gapOpen;
			e_new = e_last + sp->gapExt;
			f_new = f_last + sp->gapExt;
			//		ee.ddata[i][j] = MAX(e_new, l_new);
			//		ff.ddata[i][j] = MAX(f_new, u_new);

			if (e_new == d_new)
				ee.ddata[i][j] = l_new;
			else
				ee.ddata[i][j] = MAX(e_new, l_new);
			if (f_new == d_new)
				ff.ddata[i][j] = u_new;
			else
				ff.ddata[i][j] = MAX(f_new, u_new);

			score_mat.ddata[i][j] = MAX(MAX(d_new, ee.ddata[i][j]), MAX(ff.ddata[i][j], 0));
			/*		if (d_new == e_new)
						ee.ddata[i][j] = l_new;
					if (d_new == f_new)
						ff.ddata[i][j] = u_new;
						*/

		}
	}

	element_t score = find_max(&score_mat);
	//	print_matrix(&score_mat);
	//	print_matrix(&ee);
	//	print_matrix(&ff);
	free_matrix(&score_mat);
	free_matrix(&ee);
	free_matrix(&ff);
	return score.d;
}


region_t sw_alignment_swipe(const search_swag_profile_t * sp, const sequence_t *xseq, const sequence_t *yseq) {
	region_t region;
	double score = 0; /* score of direct path*/
	double nick = 0; /* score of backward path*/
	size_t size = MAX(xseq->len, yseq->len);
	double *EE = malloc(size * sizeof(double));
	double *HH = malloc(size * sizeof(double));
	double v;

	/* direct aisle */
	for (size_t j = 0; j < xseq->len; j++) {
		HH[j] = 0; /* memset */
		EE[j] = sp->gapOpen;
	}

	for (size_t i = 0; i < yseq->len; i++) {
		double h = 0;
		double p = 0;
		double f = sp->gapOpen;

		for (size_t j = 0; j < xseq->len; j++) {
			f = MAX(f, h + sp->gapOpen) + sp->gapExt;
			EE[j] = MAX(EE[j], HH[j] + sp->gapOpen) + sp->gapExt;

			if (!sp->mtx)
				v = SCORE(xseq->seq[j], yseq->seq[i], 1.0, -1.0);
			else
				v = VDTABLE(xseq->seq[j], yseq->seq[i]);
			h = p + v;
			if (h < 0) h = 0;
			if (f > h) h = f;
			if (EE[j] > h) h = EE[j];
			p = HH[j];
			HH[j] = h;
			if (h > score) {
				score = h;
				region.fdscore = score;
				region.x_f = j;
				region.y_f = i;
			}
		}
	}
	/* backward pass */
	for (size_t j = 0; j < region.y_f + 1; j++) {
		HH[j] = -1;
		EE[j] = -1;
	}

	for (size_t i = 0; i < region.y_f + 1; i++) {
		size_t bi = region.y_f - i;
		double h = -1;
		double f = -1;
		double p = (i != 0) ? -1 : 0;

		for (size_t j = 0; j < region.x_f + 1; j++) {
			size_t bj = region.x_f - j;
			f = MAX(f, h + sp->gapOpen) + sp->gapExt;
			EE[bj] = MAX(EE[bj], HH[bj] + sp->gapOpen) + sp->gapExt;

			if (!sp->mtx)
				v = SCORE(xseq->seq[bj], yseq->seq[bi], 1.0, -1.0);
			else
				v = VDTABLE(xseq->seq[bj], yseq->seq[bi]);

			h = p + v;

			if (f > h) h = f;
			if (EE[bj] > h) h = EE[bj];
			p = HH[bj];
			HH[bj] = h;

			if (h > nick) {
				nick = h;
				region.x_s = bj;
				region.y_s = bi;
				if (nick >= score) {
					region.bdscore = nick;
					free(EE);
					free(HH);
					return region;
				}
			}
		}
	}
	report_warning("check in align function.");
	free(EE);
	free(HH);
	return (region_t) { 0, 0, 0, 0, 0, 0 };
}

#define LAL_MASK_MISMATCH      (1<<0)
#define LAL_MASK_MATCH         (1<<1)
#define LAL_MASK_GAP_OPEN_UP   (1<<2)
#define LAL_MASK_GAP_OPEN_LEFT (1<<3)
#define LAL_MASK_GAP_EXT_UP    (1<<4)
#define LAL_MASK_GAP_EXT_LEFT  (1<<5)
#define LAL_MASK_ZERO          (1<<6)

score_matrix_t sw_directions(const search_swag_profile_t * sp, const sequence_t *xseq, const sequence_t *yseq) {
	dbg_print("[sw] call sw_directions:val %s, in f: %s,l: %d\n", xseq->seq, __FILE__, __LINE__);
	dbg_print("[sw] call sw_directions:val %s, in f: %s,l: %d\n", yseq->seq, __FILE__, __LINE__);
	double h; /* current value */
	double n; /* diagonally previous value */
	double e; /* value in left cell */
	double f; /* value in upper cell */
	double * scoprev_upprev = malloc(2 * xseq->len * sizeof(double)); /* scoprev_upprev holds in the first column the scores of the previous column and in the second column the gap-values of the previous column */
	double *su_p; /* pointer to element of scoprev_upprev */
	matrix_t score_mat = matrix(yseq->len + 1, xseq->len + 1, DOUBLETYPE); // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t directions_mat = matrix(yseq->len + 1, xseq->len + 1, CHARTYPE); /* */
	if (!score_mat.ddata) return (score_matrix_t) { 0, 0 };
	if (!directions_mat.ddata) { free_matrix(&score_mat); return (score_matrix_t) { 0, 0 }; }

	matrix_set(&score_mat, (element_t) { 0, DOUBLETYPE });
	matrix_set(&directions_mat, (element_t) { 0, CHARTYPE });

	memset(scoprev_upprev, 0, 2 * xseq->len * sizeof(double));

	for (size_t j = 0; j < yseq->len; j++) {
		su_p = scoprev_upprev;
		f = 0;   /* value in first upper cell */
		h = 0;   /* value in first cell of line */

		for (size_t i = 0; i < xseq->len; i++) {
			double score_max = 0.0, score_up = 0.0, score_left = 0.0, score_mid = 0.0;
			int direct;
			element_t el_direction = (element_t) { 0, CHARTYPE };
			element_t el_score = (element_t) { 0, DOUBLETYPE };
			double v;
			n = *su_p;
			e = *(su_p + 1);
			direct = (yseq->seq[j] == xseq->seq[i]) ? (LAL_MASK_MATCH) : (LAL_MASK_MISMATCH);
			if (!sp->mtx) {
				v = SCORE(yseq->seq[j], xseq->seq[i], 1.0, -1.0);
			}
			else {
				v = VDTABLE(yseq->seq[j], xseq->seq[i]);
			}

			h += v;

			/* stow for gap opening */
			if (f > h) {
				el_direction.c = LAL_MASK_GAP_OPEN_UP;
				h = f;
			}
			if (e > h) {
				h = e;
				el_direction.c = LAL_MASK_GAP_OPEN_LEFT;
			}

			if (h < 0) {
				h = 0;
				el_direction.c = LAL_MASK_ZERO;
			}

			*su_p = h;
			score_mid = h;

			/* stow for gap extensions */
			h += sp->gapOpen + sp->gapExt;
			e += sp->gapExt;
			f += sp->gapExt;

			score_up = f;
			score_left = e;

			if (f > h) {
				el_direction.c |= LAL_MASK_GAP_OPEN_UP; /*LAL_MASK_GAP_EXT_UP*/
			}
			else {
				f = h;
			}

			if (e > h) {
				el_direction.c |= LAL_MASK_GAP_OPEN_LEFT;/*LAL_MASK_GAP_EXT_LEFT; */
			}
			else {
				e = h;
			}

			// reset 
			el_direction.c = 0;
			score_max = MAX(score_mid, MAX(score_up, MAX(score_left, 0.0)));
			if (score_max == score_mid)
				el_direction.c = direct;
			if (score_max == score_left)
				el_direction.c |= (1 << 2);
			if (score_max == score_up)
				el_direction.c |= (1 << 3);
			if (score_max == 0)
				el_direction.c |= (1 << 6);

			matrix_or_bitwise(&directions_mat, j + 1, i + 1, el_direction); /*can be changed on set_value method*/
			el_score.d = score_max;
			matrix_set_value(&score_mat, j + 1, i + 1, el_score);

			/* next round of swapping */
			*(su_p + 1) = e;
			h = n;
			su_p += 2;
		}
	}

	free(scoprev_upprev);

	return (score_matrix_t) { score_mat, directions_mat };
}


score_matrix_t sw_genc_directions(const search_swag_profile_t * sp, const sequence_t * dseq, const sequence_t * qseq) {
	dbg_print("[sw] call sw_genc_directions: len %zd, in f: %s,l: %d\n", dseq->len, __FILE__, __LINE__);
	dbg_print("[sw] call sw_genc_directions: len %zd, in f: %s,l: %d\n", qseq->len, __FILE__, __LINE__);
	double v;
	matrix_t h = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE); // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t ee = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);        // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t ff = matrix(dseq->len + 1, qseq->len + 1, DOUBLETYPE);        // todo: there is a bug. we have to use calloc instead of malloc 
	matrix_t directions_mat = matrix(dseq->len + 1, qseq->len + 1, CHARTYPE); /* */

	if (!h.ddata) return(score_matrix_t) { 0, 0 };
	if (!ee.ddata) { free_matrix(&h); return (score_matrix_t) { 0, 0 }; }
	if (!ff.ddata) { free_matrix(&ee); free_matrix(&h); return (score_matrix_t) { 0, 0 }; }
	if (!directions_mat.ddata) { free_matrix(&ff); free_matrix(&ee); free_matrix(&h); return (score_matrix_t) { 0, 0 }; }

	matrix_set(&h, (element_t) { 0, DOUBLETYPE });
	matrix_set(&ee, (element_t) { 0, DOUBLETYPE });
	matrix_set(&ff, (element_t) { 0, DOUBLETYPE });
	matrix_set(&directions_mat, (element_t) { 0, CHARTYPE });

	double *hr0 = malloc((qseq->len + 2) * sizeof(double));
	if (!hr0) { free_matrix(&ff); free_matrix(&ee); free_matrix(&h); free_matrix(&directions_mat); return (score_matrix_t) { 0, 0 }; }
	double *fr0 = malloc((qseq->len + 2) * sizeof(double));
	if (!fr0) { free_matrix(&ff); free_matrix(&ee); free_matrix(&h); free_matrix(&directions_mat); free(hr0); return (score_matrix_t) { 0, 0 }; }
	double *hr1 = malloc((qseq->len + 2) * sizeof(double));
	if (!hr1) { free_matrix(&ff); free_matrix(&ee); free_matrix(&h); free_matrix(&directions_mat); free(hr0); free(fr0); return (score_matrix_t) { 0, 0 }; }
	double *fr1 = malloc((qseq->len + 2) * sizeof(double));
	if (!fr1) { free_matrix(&ff); free_matrix(&ee); free_matrix(&h); free_matrix(&directions_mat); free(hr0); free(fr0); free(hr1); return (score_matrix_t) { 0, 0 }; }

	for (size_t j = 0; j < qseq->len + 2; j++) {
		hr0[j] = 0;
		fr0[j] = 0;
		hr1[j] = 0;
		fr1[j] = 0;
	}

	double *hr0p = hr0;
	double *fr0p = fr0;
	double *hr1p = hr1;
	double *fr1p = fr1;

	for (size_t i = 1; i < dseq->len + 1; i++) {
		double er = 0;
		double ern0 = -sp->gapExt;
		double hr = 0;
		for (size_t j = 1; j < qseq->len + 1; j++) {
			double ern;
			if (!sp->mtx)
				v = SCORE(dseq->seq[i - 1], qseq->seq[j - 1], 1.0, -1.0);
			else
				v = VDTABLE(dseq->seq[i - 1], qseq->seq[j - 1]);
			element_t el = (element_t) { 0, DOUBLETYPE };
			double  mx_new = hr0p[j + 1] + sp->gapOpen;
			ern0 = MAX(ern0 + sp->gapExt, hr0p[j - 1] + sp->gapOpen);
			ern = el.d = MAX(er + sp->gapExt, hr + sp->gapOpen);
			matrix_set_value(&ee, i, j, el);
			hr = MAX(hr0p[j], MAX(ern0, fr0p[j - 1]));
			hr += v;
			hr = MAX(hr, 0);
			hr1p[j + 1] = el.d = hr;
			matrix_set_value(&h, i, j, el);
			fr1p[j] = el.d = MAX(fr0p[j] + sp->gapExt, mx_new);
			matrix_set_value(&ff, i, j, el);
			er = ern;

			{
				element_t el_direction = (element_t) { 0, CHARTYPE };
				if (hr == (hr0p[j] + v /* == m_new */) && v > 0) /*m_new*/
					el_direction.c |= (1 << 1);// #define LAL_MASK_MATCH         (1<<1)
				if (hr == (hr0p[j] + v) && v <= 0) /* m_new */
					el_direction.c |= (1 << 0);// #define LAL_MASK_MISMATCH      (1<<0)
				if (hr == fr0p[j - 1] + v)
					el_direction.c |= (1 << 2); // #define LAL_MASK_GAP_OPEN_LEFT (1<<3)
				if (hr == ern0 + v)
					el_direction.c |= (1 << 3);// #define LAL_MASK_GAP_OPEN_UP   (1<<2)
				if (hr == 0)
					el_direction.c |= (1 << 6); // #define LAL_MASK_ZERO          (1<<6)
			}
		}
		double *tf = fr0p;
		double *th = hr0p;
		fr0p = fr1p;
		hr0p = hr1p;
		fr1p = tf;
		hr1p = th;
	}
	element_t score = find_max(&h);
	dbg_print("[sw] call sw_genc_directions: score %f, in f: %s,l: %d\n", score.d, __FILE__, __LINE__);

	free_matrix(&ee);
	free_matrix(&ff);
	free(hr0);
	free(fr0);
	free(hr1);
	free(fr1);
	return (score_matrix_t) { h, directions_mat };
}

double sw_genc_sigaev(const search_swag_profile_t * sp, const sequence_t * dseq, const sequence_t * qseq) {
	dbg_print("[sw] call sw_genc_sigaev: len %zd, in f: %s,l: %d\n", dseq->len, __FILE__, __LINE__);
	dbg_print("[sw] call sw_genc_sigaev: len %zd, in f: %s,l: %d\n", qseq->len, __FILE__, __LINE__);
	double v;

	double *hr0 = malloc((qseq->len + 2) * sizeof(double));
	if (!hr0) { return 0.0; }
	double *fr0 = malloc((qseq->len + 2) * sizeof(double));
	if (!fr0) { free(hr0); return 0.0; }
	double *hr1 = malloc((qseq->len + 2) * sizeof(double));
	if (!hr1) { free(hr0); free(fr0); return 0.0; }
	double *fr1 = malloc((qseq->len + 2) * sizeof(double));
	if (!fr1) { free(hr0); free(fr0); free(hr1); 0.0; }

	for (size_t j = 0; j < qseq->len + 2; j++) {
		hr0[j] = 0;
		fr0[j] = 0;
		hr1[j] = 0;
		fr1[j] = 0;
	}

	double *hr0p = hr0;
	double *fr0p = fr0;
	double *hr1p = hr1;
	double *fr1p = fr1;

	double score = 0.0;

	for (size_t i = 1; i < dseq->len + 1; i++) {
		double er = 0;
		double ern0 = -sp->gapExt;
		double hr = 0;
		for (size_t j = 1; j < qseq->len + 1; j++) {
			double ern;
			if (!sp->mtx)
				v = SCORE(dseq->seq[i - 1], qseq->seq[j - 1], 1.0, -1.0);
			else
				v = VDTABLE(dseq->seq[i - 1], qseq->seq[j - 1]);
			double  mx_new = hr0p[j + 1] + sp->gapOpen;
			ern0 = MAX(ern0 + sp->gapExt, hr0p[j - 1] + sp->gapOpen);
			ern = MAX(er + sp->gapExt, hr + sp->gapOpen);
			hr = MAX(hr0p[j], MAX(ern0, fr0p[j - 1]));
			hr += v;
			hr = MAX(hr, 0);
			hr1p[j + 1] = hr;
			fr1p[j] = MAX(fr0p[j] + sp->gapExt, mx_new);
			er = ern;
			score = MAX(score, hr);
		}
		double *tf = fr0p;
		double *th = hr0p;
		fr0p = fr1p;
		hr0p = hr1p;
		fr1p = tf;
		hr1p = th;
	}
	dbg_print("[sw] call sw_genc_directions: score %f, in f: %s,l: %d\n", score, __FILE__, __LINE__);

	free(hr0);
	free(fr0);
	free(hr1);
	free(fr1);
	return score;
}