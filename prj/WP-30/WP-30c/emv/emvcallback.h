#ifndef EMVCALLBACK_H_
#define EMVCALLBACK_H_

int emv_get_transaction_amount(void);
int emv_account_type_select(void);
int emv_select_candidate_application(void);
int emv_get_cardholder_pin(PINSTATUS *pEncryptInfo, uchar *CardPin);
int emv_show_pin_verify_result(uchar TryCounter);
int emv_process_online_pin(PINSTATUS *pEncryptInfo);
int emv_process_pboc_certificate(void);
int emv_process_advice(void);
int emv_process_online(uchar *RspCode,
	                         uchar *AuthCode, int *AuthCodeLen,
	                         uchar  *IAuthData,int *IAuthDataLen,
	                         uchar *script, int *ScriptLen);
int emv_process_issuer_refer(void);
int emv_set_transaction_data(void);
int emv_ec_account_select(void);


#endif

