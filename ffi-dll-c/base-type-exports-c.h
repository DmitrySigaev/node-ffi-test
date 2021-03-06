/* base-type-exports-c.h
 * Copyright 2016 Dmitry Sigaev
 *
 * Released under the MIT license -- see MIT-LICENSE for details
 */

#if !defined(__BASE_TYPE_EXPORTS_C_H__)
#define __BASE_TYPE_EXPORTS_C_H__ 

#if defined(FFIDLL_EXPORTS)
#define FFIEXPORT extern __declspec(dllexport)
#else
#define FFIEXPORT extern __declspec(dllimport)
#endif

#include <stdint.h>

FFIEXPORT void voidFunc(void);
FFIEXPORT char charFunc(char inChar, char *outChar);
FFIEXPORT float floatFunc(float inFloat, float *outFloat);
FFIEXPORT double doubleFunc(double inDouble, double *outDouble);
FFIEXPORT bool boolFunc(bool inBool, bool *outBool);
FFIEXPORT wchar_t wcharFunc(wchar_t inWchar, wchar_t *outWchar);

struct tag_element_api {
	union {
		double d;
		int64_t i;
		int8_t  c;
	};
	enum MATTYPE type;
};

struct tag_matrix_api {
	double **ddata;
	size_t nrows;
	size_t ncols;
	int type;
};
FFIEXPORT struct tag_element_api find_max_js(struct tag_matrix_api *matrix);
FFIEXPORT struct tag_matrix_api matrix_js(const size_t nrows, const size_t ncols, int type);
FFIEXPORT struct tag_matrix_api *matrix_js_d(const size_t nrows, const size_t ncols, int type);
FFIEXPORT int *matrix_js_i(const size_t nrows, const size_t ncols, int type);
FFIEXPORT int matrix_set_int(struct tag_matrix_api *matrix, const int value);
FFIEXPORT int matrix_set_double(struct tag_matrix_api *matrix, const double value);
FFIEXPORT int matrix_set_char(struct tag_matrix_api *matrix, const char value);

#define MAX_LINE_LEN_API  1024
#define MAX_DOC_LEN_API   (MAX_LINE_LEN_API*4)

struct tag_scoring_matrix_api {
	struct tag_matrix_api sc_double_matrix;
	struct tag_matrix_api sc_int_matrix;
	double scale;
	double scaleback;  /* 1.0/scale */
	double man2mip[3];
	double gapOpen, gapExtend;
	char Doc[MAX_DOC_LEN_API];
};

FFIEXPORT int read_scoring_matrix_js(struct tag_scoring_matrix_api * mtx, const char *matrixstring, size_t len);

struct  search_swag_profile_api {
	double gapOpen;
	double gapExt;
	struct tag_scoring_matrix_api *mtx;
};

struct sequence_api {
	size_t ID;
	char * seq;
	size_t len;
};

struct score_matrix_api {
	struct tag_matrix_api score;
	struct tag_matrix_api directions;
};

FFIEXPORT struct score_matrix_api sw_directions_js(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq);
FFIEXPORT void encode_seq_js(struct sequence_api const *s, struct sequence_api const *d);

FFIEXPORT double sw_genc_js(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq);
FFIEXPORT struct score_matrix_api sw_genc_m_js(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq);

FFIEXPORT double sw_genc_js_sigaev(struct  search_swag_profile_api const * sp, struct sequence_api const *xseq, struct sequence_api const *yseq);

FFIEXPORT int lal_seq_base_create_js(void);
FFIEXPORT void lal_seq_base_close_js(void);
FFIEXPORT int lal_add_fasta_js(const char * pathname);
FFIEXPORT int lal_add_string_js(const char * string);

FFIEXPORT size_t fasta_get_sequence_count_js(void);
FFIEXPORT size_t fasta_get_symbol_count_js(void);
FFIEXPORT size_t fasta_get_longest_sequence_js(void);
FFIEXPORT struct sequence_api const * fasta_get_longest_seq_struct_js(void);
FFIEXPORT struct sequence_api const * fasta_get_seq_struct_js(size_t seqno);

#define CALLBACKFFI
typedef void (CALLBACKFFI *FFIPROC)(void);

struct tagffiAPI
{
	FFIPROC voidFunc;
	char(*charFunc)(char, char *);
	float(*floatFunc)(float, float *);
	double(*doubleFunc)(double, double *);
	bool(*boolFunc)(bool, bool *);
	wchar_t(*wcharFunc)(wchar_t, wchar_t *);
	int(*read_scoring_matrix_func)(struct tag_scoring_matrix_t *mtx, const char *matrixstring, size_t len);
};
FFIEXPORT struct tagffiAPI LoadFFI(void);

#endif /* defined(__BASE_TYPE_EXPORTS_H__) */
