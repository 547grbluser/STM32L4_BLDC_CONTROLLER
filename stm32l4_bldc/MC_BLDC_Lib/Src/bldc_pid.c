#include "bldc_pid.h"
#include "main.h"

void MC_SixStep_Set_PI_Param(stSIXSTEP_PI_Param *PI_Param)
{
		PI_Param->ReferenceSpeed = 0;   
		PI_Param->integralTermSum = 0;
			
		PI_Param->Kp_Gain = KP_GAIN;   
		PI_Param->Ki_Gain = KI_GAIN;      

		PI_Param->Lower_Limit_Output = LOWER_OUT_LIMIT;       
		PI_Param->Upper_Limit_Output = UPPER_OUT_LIMIT;     

		PI_Param->Max_PID_Output =  FALSE; 
		PI_Param->Min_PID_Output =  FALSE;
}


uint16_t MC_SixStep_PI_Controller(stSIXSTEP_PI_Param *PI_Param, uint16_t speedFdbk)
{
		int32_t wProportional_Term=0, wIntegral_Term=0, wOutput_32=0,wIntegral_sum_temp=0;
		int32_t Error =0;
			
		Error = (PI_Param->ReferenceSpeed - speedFdbk);
		
		/* Proportional term computation*/
		wProportional_Term = PI_Param->Kp_Gain * Error;
			
		/* Integral term computation */
		if (PI_Param->Ki_Gain == 0)
		{
				PI_Param->integralTermSum = 0;
		}
		else
		{ 
				wIntegral_Term = PI_Param->Ki_Gain * Error;
				wIntegral_sum_temp = PI_Param->integralTermSum + wIntegral_Term;
				PI_Param->integralTermSum = wIntegral_sum_temp;
		}
  

    if(PI_Param->integralTermSum> KI_DIV * PI_Param->Upper_Limit_Output)
		{
       PI_Param->integralTermSum = KI_DIV* PI_Param->Upper_Limit_Output;
		}
   
    if(PI_Param->integralTermSum<-KI_DIV* PI_Param->Upper_Limit_Output)
		{
				PI_Param->integralTermSum = -KI_DIV* PI_Param->Upper_Limit_Output;
		}
    

               
    wOutput_32 = (wProportional_Term/KP_DIV) + (PI_Param->integralTermSum/KI_DIV);

  

	 if (wOutput_32 > PI_Param->Upper_Limit_Output)
	 {
				wOutput_32 = PI_Param->Upper_Limit_Output;		  			 	
	 }
	 else if (wOutput_32 < PI_Param->Lower_Limit_Output)
	 {
				wOutput_32 = PI_Param->Lower_Limit_Output;
	 }


		return((uint16_t)(wOutput_32)); 	
}
