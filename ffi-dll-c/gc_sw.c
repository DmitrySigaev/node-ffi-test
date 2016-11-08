/*
Copyright (C) 2016

Contact: Dmitry Sigaev <dima.sigaev@gmail.com>
*/

#include <stdio.h>
#include <malloc.h>
#include <limits.h>
#include <math.h>

#include "lal_matrix.h"
#include "lal_report.h"
#include "gc_sw.h"

/* type of the profile and calc score for MS_ONE */
typedef int scoretype;

typedef struct {
	scoretype *data; /* array as if of [prof_length][this.line_size]*/
	int line_size;   /* size of the profile line */
					 /*  float scaleback;*/ /* factor to scale results back to float */
	double scaleback; /* factor to scale results back to float */
} ms_profile_t;

/* Type of the best result, current in and returned by the
internal score_calc function.
Contains score and end coordinates of best alignment.
*/
typedef struct best {
	scoretype score; /* alignment score */
					 /*  int state;*/
	int x; /* alignment's end coordinates */
	int y;
} best_t;

/* length of the match table section of a profile line.
In HMM2 these are match and insert sections of this length.
There are 32 scores in match section for 32 letters and symbols possible
in a sequence */
#define MS_PROFILE_MATCHTABLE_SIZE 32

/* each profile line id 38 floats
( 32 emission scores + 6 gap penalties : gapop / gapext / qgapop / qgapext / begin / end ) */
#define PROFILE_LINE_SIZE 38

/* accessor for element j in line i of profile pr */
#define PROFILE_VAL(pr,i,j) pr[(i)*PROFILE_LINE_SIZE+(j)]

#define PROFILE_EMISSIONS_NO 32
#define PROFILE_GAPOP   32
#define PROFILE_GAPEXT  33
#define PROFILE_QGAPOP  34
#define PROFILE_QGAPEXT 35
#define PROFILE_BEGIN   36
#define PROFILE_END     37


enum MS_SW_OFFSETS {
	MS_SW_PROF_SCORES = 0,
	MS_SW_OFFSET_GAPOP = MS_SW_PROF_SCORES + MS_PROFILE_MATCHTABLE_SIZE,
	MS_SW_OFFSET_GAPEX,
	MS_SW_OFFSET_GAPOP1,
	MS_SW_OFFSET_GAPEX1,
	MS_SW_OFFSET_DEPTH /* indicates depth in the data4vec profile */
};

#define min_2(a, b)  (((a)<(b)) ? (a) : (b))/* b will be evaluated twice! */

/* Used as minus infinity in creating profile when a score of below
MS_MIN_PROFILE_CUTOFF is found in GenProf.
Also used in initializing transitions to minus infinity.
Type: scoretype
Need to be able to add 8 times (gw9)
Should be able to beat any MS_ONE_MAX_TOTAL
*/
#define MS_SCORE_MININF    ((scoretype)(INT_MIN/16))

/* Type of the sequence element.
Code of letter: (capital_char - 'A')
Range: 0..31
Used as offset in matchtable part of the profile line to get match score */
typedef int seqtype;

/* As test moved into the start of the code, it tests old quality,
that comes from a diagonal cell (x-1, y-1). This adjustment will
be done in the very end. */
#define checkbest(quality, xx, yy, max_v) \
                           if ((quality)>max_v.score) {\
	                       max_v.score=(quality);  \
         	               max_v.x=(xx);            \
	                       max_v.y=(yy);            \
                           }

typedef struct prev_line_s {
	scoretype yskipmatch; /* delete state */
	scoretype prequal; /* match state */
	seqtype   nseq; /* letter of the sequence */
} prev_line_t;

static best_t MS_Score_SW(seqtype* nseq, ms_profile_t *prof, int profLen, int seqLen) {
	register int x = 0, y = 0; /* x - coordinate in seq, y - coord in profile */

	register scoretype xskipmatch = 0; /* insert state */
	register scoretype quality = 0; /* match state */

	prev_line_t *prev_line; /* array that stores state scores for whole line of DP matrix */
	register prev_line_t *pre; /* pointer in prev_line array */

	register scoretype lastquality; /* tmp buffer to save previous quality score
									until we can save it into prev_line */
	register scoretype yskipmatch; /* delete state */
	scoretype score; /* match score from profile*/

	scoretype penopen, penext, penopen1, penext1;/* gap penalties: for x and y */

	register scoretype(*prof_line_p); /* profile data pointer */
									  /*register scoretype bestjump,bestjump1;*/

	register scoretype pentest;

#define _COUNT 0
#if _COUNT
	static int count1 = 1, count2 = 1, count3 = 1, count12 = 1, count13 = 1, count123 = 1, count0 = 2;
#endif

	/*  FILE *fp; not used */
	/*int dum1=0,dum2=0;*/
	best_t max_v; /* best seen alignment score in a match state */

	max_v.score = MS_SCORE_MININF;
	max_v.x = -1;
	max_v.y = -1;

	/* db sequences come padded with a spare base at the beginning
	(N for nucleic, X for protein sequences). We skip it here. */
	seqLen--;
	nseq++;

	prev_line = (prev_line_t *)calloc((seqLen + 1), sizeof(prev_line_t));

	prev_line[seqLen].nseq = -1; /* end of sequence token */

								 /* init prev_line by the scores for first line in the profile */
	for (x = 0, pre = prev_line, prof_line_p = prof->data; x < seqLen; x++, pre++)
	{
		pre->nseq = nseq[x];
		pre->prequal = prof_line_p[pre->nseq];
		/*if( pre->prequal<0 ) pre->prequal=0;
		else */ checkbest(pre->prequal, x + 1, 1, max_v);
		pre->yskipmatch = MS_SCORE_MININF;
	}

	/* loop on profile lines 1..(profLen-1) */
	/* prof_line_p already set to prof->data */
	for (y = 1; y < profLen; y++)
	{
		pre = prev_line; /* pointer to line of state scores in DP matrix */

						 /* gap penalties with '1' for line 'n' are kept in line 'n-1' */
		penopen1 = prof_line_p[MS_SW_OFFSET_GAPOP1];
		penext1 = prof_line_p[MS_SW_OFFSET_GAPEX1];

		/* next line */
		prof_line_p += prof->line_size;

		quality = prof_line_p[pre->nseq];
		/*if(quality<0) quality=0;
		else*/ checkbest(quality, 1, y + 1, max_v);
		lastquality = quality;

		xskipmatch = MS_SCORE_MININF;
		penopen = prof_line_p[MS_SW_OFFSET_GAPOP];
		penext = prof_line_p[MS_SW_OFFSET_GAPEX];

		/* take the smaller positive value */
		/* SWAT1 */pentest = min_2(-penopen, -penopen1);
		pre++;

		/* loop on sequence positions */
		/* 1..(seqLen-1), but prequal and yskip are virtually one step behind:
		0..(seqLen-2) */
		while (pre->nseq >= 0)
		{
			score = prof_line_p[pre->nseq]; /* profile value for match */
			quality = pre->prequal;
			yskipmatch = pre->yskipmatch;

#if _COUNT
			count0++;
			if (xskipmatch <= 0) count1++;
			if (yskipmatch <= 0) count2++;
			if (xskipmatch <= 0 && yskipmatch <= 0) count12++;
			if (quality <= pentest) count3++;
			if (quality <= pentest && yskipmatch <= 0) count13++;
			if (xskipmatch <= 0 && yskipmatch <= 0 && quality <= pentest) count123++;
#endif

#define _SWATS 1
#if _SWATS
			if (yskipmatch <= 0) {
				if (xskipmatch <= 0) { /* yskipmatch <= 0 and xskipmatch <= 0 */

					if (quality <= 0) {
						quality = 0;
					}
					else {
						if (quality > pentest) {
							xskipmatch = quality + penopen;
							pre->yskipmatch = quality + penopen1;
						}
						checkbest(quality, pre - prev_line, y, max_v)
					}

				}
				else {               /* yskipmatch <= 0 && xskipmatch > 0 */
					if (quality <= 0) {
						quality = xskipmatch;
						xskipmatch += penext;
					}
					else {
						register scoretype bestjump, bestjump1;
						bestjump1 = quality + penopen1;
						/*if (bestjump1 > 0)*/ pre->yskipmatch = bestjump1;

						if (quality < xskipmatch) { /* q<0 handled here */
							quality = xskipmatch;
							xskipmatch += penext;  /* assuming penopen<penext @@ test input */
						}
						else {
							xskipmatch += penext;
							bestjump = quality + penopen;
							if (bestjump > xskipmatch) xskipmatch = bestjump;
							checkbest(quality, pre - prev_line, y, max_v)
						}
					}

				}
			}
			else { /* yskip > 0 */
				if (xskipmatch <= 0) {  /* yest > 0 and xskipmatch <= 0 */
					if (quality <= 0) {
						quality = yskipmatch;
						yskipmatch += penext1;
					}
					else {
						register scoretype bestjump, bestjump1;
						bestjump = quality + penopen;
						/*if (bestjump > 0)*/ xskipmatch = bestjump;

						if (quality < yskipmatch) { /* q<0 handled here */
							quality = yskipmatch;
							yskipmatch += penext1; /* assuming penopen1<penext1 */
						}
						else {
							yskipmatch += penext1;
							bestjump1 = quality + penopen1;
							if (bestjump1 > yskipmatch) yskipmatch = bestjump1;
							checkbest(quality, pre - prev_line, y, max_v)
						}
					}

				}
				else { /* xskipmatch > 0 && yskipmatch > 0 */

					if (quality < xskipmatch) {

						if (quality < yskipmatch) { /* q<0 handled here */
							if (yskipmatch > xskipmatch) quality = yskipmatch;
							else                  quality = xskipmatch;
							yskipmatch += penext1; /* assuming penopen1<penext1 */
						}
						else {
							register scoretype bestjump1;
							yskipmatch += penext1;
							bestjump1 = quality + penopen1;
							if (bestjump1 > yskipmatch) yskipmatch = bestjump1;
							quality = xskipmatch;
						}

						xskipmatch += penext;  /* assuming penopen<penext */
					}
					else { /* quality>=xskipmatch */
						register scoretype bestjump, bestjump1;
						xskipmatch += penext;
						bestjump = quality + penopen;
						if (bestjump > xskipmatch) xskipmatch = bestjump;


						if (quality < yskipmatch) {
							quality = yskipmatch;
							yskipmatch += penext1; /* assuming penopen1<penext1 */
						}
						else {
							checkbest(quality, pre - prev_line, y, max_v)
								yskipmatch += penext1;
							bestjump1 = quality + penopen1;
							if (bestjump1 > yskipmatch) yskipmatch = bestjump1;
						}

					} /* quality>=xskipmatch */

				} /* yskipmatch test */

				pre->yskipmatch = yskipmatch;
			} /* xskipmatch test */

#else /* _SWATS */

			/* first optimization SWAT1 */
			/**
			if (xskipmatch <= 0 && yskipmatch <= 0 && quality<=pentest) {
			#if _COUNT
			count1++;
			#endif
			} else {
			* */
			{
				bestjump = quality + penopen;
				bestjump1 = quality + penopen1;
				checkbest(quality, pre - prev_line, y, max_v)

					/* second optimization SWAT2 */
					/**/ if (xskipmatch <= 0 && yskipmatch <= 0) {
						xskipmatch = bestjump;
						yskipmatch = bestjump1;
					}
					else
					{

						/*  */
						if (quality >= xskipmatch) {
							if (quality < yskipmatch) quality = yskipmatch;
						}
						else if (xskipmatch >= yskipmatch) quality = xskipmatch;
						else                         quality = yskipmatch;



						xskipmatch += penext;
						if (bestjump >= xskipmatch)
							xskipmatch = bestjump;

						yskipmatch += penext1;
						if (bestjump1 >= yskipmatch)
							yskipmatch = bestjump1;
					}
			}
			pre->yskipmatch = yskipmatch;
			if (quality < 0) quality = 0;
#endif /* _SWATS */

			quality += score;
			/*optimize out @@ if(quality<0) quality=0;
			else */

			pre->prequal = lastquality;
			pre++;
			lastquality = quality;
		} /* sequence loop */
		checkbest(quality, seqLen, y + 1, max_v)

	} /* profile loop */

	for (x = 1, pre = prev_line + 1; x < seqLen; x++)
	{
		checkbest(pre->prequal, x, profLen, max_v)
			pre++;
	}

	/*max_v.x--;*//*since we skip the spare base at the beginning, we don't need to adjust the index anymore here */
	max_v.y--;

	free(prev_line);

	{
#if _COUNT    
		float a1, a2, a3, a12, a13;
		a1 = 1.0*count1 / count0;
		a2 = 1.0*count2 / count0;
		a12 = 1.0*count12 / count0;
		a3 = 1.0*count13 / count0;
		a13 = 1.0*count13 / count0;
		printf("\n\nSWAT:\ncount 1x\t%.2f%% out of %d, %d\n", (100.0*a1), count0, count1);
		printf("count 2y\t%.2f%% out of %d, %d\n", (100.0*a2), count0, count2);
		printf("count 12xy\t%.2f%% out of %d, %d\t=c %.4f%%\n", (100.0*a12), count0, count12, 100.0*(a12 - a1*a2) / sqrt(a1*(1 - a1)*a2*(1 - a2)));
		printf("count 3q\t%.2f%% out of %d, %d\n", (100.0*a3), count0, count3);
		printf("count 13xq\t%.2f%% out of %d, %d\t=c %.4f%%\n", (100.0*a13), count0, count13, 100.0*(a13 - a1*a3) / sqrt(a1*(1 - a1)*a3*(1 - a3)));
		printf("count 123xyq\t%.2f%% out of %d, %d\n\n", (100.0*count123 / count0), count0, count123);
#endif
	}

	return max_v;
	} /* MS_Score_SW() */

#define MIN_DSP_NEGATIVE -1000
  /*** Gencore profile ***/
  /* min_inf in GenProf profiles.
  This value is replaced with MS_SCORE_MININF in MS_ONE Profiles
  Set in Gencore to (-1000) */
#define MS_MIN_PROFILE_CUTOFF MIN_DSP_NEGATIVE


  /* convert x into scoretype value sTmp1, and compute relative error:
  if (x != 0)
  if (x-sTmp1) != 0)
  if (fabs((x-sTmp1)/x) > max_rel_error)
  max_rel_error = fabs((x-sTmp1)/x)

  */
#define to_scoretype_(ss)  ((scoretype)rint(ss))

static scoretype to_scoretype(float data, float scale, float *max_rel_error) {
	float ss = data * scale;
	scoretype sTmp1;

	sTmp1 = to_scoretype_(ss);
	if (ss != 0) {
		float fTmp2;
		/*     if (fabs(data) < 1e-6) {
		fTmp2 = 2048*fabs(data);
		} else */

		/* continuous function: but 0..1/2 map to 0..1, not straight 1 */
		if (sTmp1 == 0) {
			fTmp2 = 2 * fabs(ss);
		}
		else fTmp2 = fabs((ss - sTmp1) / ss);

		/* for data < 1e-6 reduce perceived error as rounding error is very big
		by now anyway. Still a continuous function. */
		if (fabs(data) < 1 / 0x10000) {
			fTmp2 *= fabs(data) * 0x10000;
		}

		if ((fTmp2 > *max_rel_error)
			/* 	 ((fabs(data) > 0.0009765625 ) /-* 1/1024 *-/ */
			/* 	 || (fabs(ss) > 0.0999 ))  /-* 10% *-/ */
			) {
			/*printf("DEBUG: to_scoretype() data=%.10f ss=%f fTmp2=%.10f max_rel_error=%.10f\n", data, ss, fTmp2, *max_rel_error); /-* debug */
			*max_rel_error = fTmp2;
		}
	}
	return sTmp1;
} /* to_scoretype()  */

  /* Return scoretype values into the new profile */
static scoretype get_scaled(float data, float scale, float *max_rel_error) {
	if (data <= MS_MIN_PROFILE_CUTOFF) {
		return MS_SCORE_MININF;
	}
	else {
		return to_scoretype(data, scale, max_rel_error);
	}
} /* get_scaled() */

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define SCORE(a, b, c, d) (((a) == (b)) ? (c) : (d))
#define VDTABLE(a, b) (sp->mtx->sc_double_matrix.ddata[(a)][(b)])
#define VITABLE(a, b) (sp->mtx->sc_int_matrix.idata[(a)][(b)])

  /*
  *structure for beginning of calculation
  * four parameters should be passed to function
  */
typedef struct inpf {
	float score;
	int state;
	int x;
	int y;
} inpf_t;

double sw_gencore(const search_swag_profile_t * sp, const sequence_t * dseq, const sequence_t * qseq)
{
	int read_base = 16;
	int line_size = 36;
	float *data_from;
	scoretype *data_to;
	float scale = sp->mtx->scale;
	float max_rel_error = 0;
	ms_profile_t * ms_profile = malloc(sizeof(ms_profile_t));
	ms_profile->scaleback = 1.0 / sp->mtx->scale;
	ms_profile->line_size = line_size;
	int prof_length = qseq->len + 1; // INT_MIN + seq(i-1)
	int prf_len = (prof_length)* PROFILE_LINE_SIZE * sizeof(float);
	float *prof_data = (float *)malloc(prf_len);

	for (size_t j = 0; j < PROFILE_EMISSIONS_NO; j++)
		PROFILE_VAL(prof_data, 0/* *32+6=38*/, j) = INT_MIN; /*sigaev: global aligment*/

															 /* gap penalties copied from first line */
	prof_data[PROFILE_GAPOP] = sp->gapOpen;
	prof_data[PROFILE_GAPEXT] = sp->gapExt;
	prof_data[PROFILE_QGAPOP] = sp->gapOpen;
	prof_data[PROFILE_QGAPEXT] = sp->gapExt;

	prof_data[PROFILE_BEGIN] = 0;  //36
	prof_data[PROFILE_END] = INT_MIN; //37
//	printf("!!!!\n");

	for (size_t i = 1; i < prof_length; i++) {
		int k = qseq->seq[i - 1];
		for (size_t j = 0; j < PROFILE_EMISSIONS_NO; j++) { //#define PROFILE_EMISSIONS_NO 32
			double v;
			if (!sp->mtx)
				v = SCORE(k, j, 1.0, -1.0);
			else
				v = VDTABLE(k, j);
//			printf(" %f, ", v);
			PROFILE_VAL(prof_data, i/* *32+6=38*/, j) = v; //create profile line from  32 CM("A") to 38 (36-39: 1,1,1,1,0,0)
		}
																				  // prof_data[(i)*PROFILE_LINE_SIZE + (j)] == prof_data[(i)*38 + (j)]
		PROFILE_VAL(prof_data, i, PROFILE_GAPOP) = sp->gapOpen;  //prof_data[(i) * 38 + 32]
		PROFILE_VAL(prof_data, i, PROFILE_GAPEXT) = sp->gapExt; //prof_data[(i) * 38 + 33]
		PROFILE_VAL(prof_data, i, PROFILE_QGAPOP) = sp->gapOpen; //prof_data[(i) * 38 + 34]
		PROFILE_VAL(prof_data, i, PROFILE_QGAPEXT) = sp->gapExt;//prof_data[(i) * 38 + 35]
		PROFILE_VAL(prof_data, i, PROFILE_BEGIN) = 0.0;  //prof_data[(i) * 38 + 36]
		PROFILE_VAL(prof_data, i, PROFILE_END) = 0.0; //prof_data[(i) * 38 + 37]
	}

	ms_profile->data = (scoretype *)malloc(sizeof(scoretype) * ms_profile->line_size * prof_length);

	{
		FILE *output;
		char strbuf[200];
		sprintf(strbuf, "%s_%0.5d", "soft_api.dbg.a.", 0);
		output = fopen(strbuf, "w");

		for (size_t i = 0; i < prof_length; i++) { /* for each profile line */

											/* First model->max_dprf profile lines are not actual lines but are added
											so that algorithm could go beyond the boundary and first lines would
											not be special cases. MS computes first lines separately. */
			data_from = &(prof_data[(i + 0/*0== SoftInfo_g.model->max_dprf*/) * PROFILE_LINE_SIZE]);
			data_to = &(ms_profile->data[i*ms_profile->line_size]);

			for (size_t j = 0; j < line_size; j++) { /* for each position in the profile */
											  /*float mre = max_rel_error;/-* debug */
				data_to[j] = get_scaled(data_from[j], scale, &max_rel_error);
				fprintf(output, "i= %zd j= %zd %f * %f -> %d\n", i, j, (double)data_from[j], scale, data_to[j]);
				//						printf(": %d %d data=%.10f mre=%.10f\n",i,j,data_from[j],max_rel_error);
				//					if (mre != max_rel_error)
				//							printf("DEBUG: MS_gcprofile2msone_profile(): %d %d data=%.10f mre=%.10f\n",i,j,data_from[j],max_rel_error);
			} /* for(j=0;j<line_size;j++) */

		} /* for(i=0;i<prof_length;i++) */

		fclose(output);
	}
	free(prof_data);

	int *ms_sequence = (int *)malloc(sizeof(int) * dseq->len+5);
	ms_sequence[0] = 13;
	for (size_t r = 0; r < dseq->len; r++) {
		ms_sequence[r+1] = dseq->seq[r];
	}
	inpf_t max_v;
	best_t max_s = MS_Score_SW(ms_sequence, ms_profile, prof_length, dseq->len+1);
	max_v.score = ((float)(max_s.score)) * ms_profile->scaleback;
	max_v.x = max_s.x;
	max_v.y = max_s.y;
	max_v.state = -1; /* unknown */
	free(ms_sequence);
	free(ms_profile->data);
	free(ms_profile);
	return max_v.score;
}