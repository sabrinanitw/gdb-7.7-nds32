#ifndef _NDS32_AIE_PIPELINE_H_
#define _NDS32_AIE_PIPELINE_H_
#include "stdlib.h"
#include "nds32-aie-utils.h"

#ifndef INIT_AIE_PIPELINE_MODEL
    #define INIT_AIE_STAGE_NUM 5
    #define INIT_AIE_ETRAP 2
    #define INIT_AIE_LTRAP 5
    #define INIT_AIE_D2M 2
    #define INIT_AIE_D2C 5
#endif

typedef struct _stage_nemonic_ stage_nemonic;
typedef struct _aie_pipe_ aie_pipe;
typedef struct _aie_cop_ aie_cop;

struct _stage_nemonic_ {
    char nemonic[MAX_STR_CONST];
    int stage;
    stage_nemonic* n;
};
//*********************************************************
//Non instruction based configuration for pipeline modeling
//*********************************************************
struct _aie_cop_ {
    int stage_num;                          //coprocessor pipeline stage number
    stage_nemonic* stage_nemonic_list;      //coprocessor pipeline stage nemonic
    stage_nemonic* etrap;                   //early trap ack stage
    stage_nemonic* ltrap;                   //late trap ack stage
    stage_nemonic* d2m;                     //data to main pipe
    stage_nemonic* d2c;                     //data to cop pipe
    int cpid;                               //coprocessor id
    char cpid_str[5];                       //string format of cpid

    //used for redef later by user
    unsigned int can_be_redef_cpid;
    unsigned int can_be_redef_d2m;
    unsigned int can_be_redef_d2c;
    unsigned int can_be_redef_etrap;
    unsigned int can_be_redef_ltrap;
    unsigned int can_be_redef_stage;

    unsigned int is_def_cpid;
    unsigned int is_def_d2m;
    unsigned int is_def_d2c;
    unsigned int is_def_etrap;
    unsigned int is_def_ltrap;
    unsigned int is_def_stage;
};
extern int is_cop_null(aie_cop* cop);
extern stage_nemonic* is_exist_stage(aie_cop* cop, char* stage);
extern int init_aie_cop(aie_cop** cop);
extern int add_cop_stage(aie_cop* cop, char* stage);
extern int add_cop_cpid(aie_cop* cop, unsigned int cpid);
extern int add_cop_etrap(aie_cop* cop, char* stage);
extern int add_cop_ltrap(aie_cop* cop, char* stage);
extern int add_cop_d2c(aie_cop* cop, char* stage);
extern int add_cop_d2m(aie_cop* cop, char* stage);

//**********************************************************************
//the following is instruction based configuration for pipeline modeling
//**********************************************************************
struct _aie_pipe_ {
    stage_nemonic* pstage;                  //produce stage
    stage_nemonic* cstage;                  //consume stage
    unsigned long long int sscycle;         //self stall cycle for blocking modeling
    stage_nemonic* ssstage;                 //self stall stage for blocking modeling
    int sreg_num;                           //source register operand number
    int dreg_num;                           //destination register operand number

    //used for redef later by user
    unsigned int can_be_redef_pstage;
    unsigned int can_be_redef_cstage;
    unsigned long long int can_be_redef_sscycle;
    unsigned int can_be_redef_ssstage;

    unsigned int is_def_pstage;
    unsigned int is_def_cstage;
    unsigned long long int is_def_sscycle;
    unsigned int is_def_ssstage;
};
extern void init_aie_pipe(aie_pipe** pipe);
#endif
