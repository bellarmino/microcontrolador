/*===============================================================================================================================================
DECLARA«√O DAS VARI¡ÅVEIS
===============================================================================================================================================*/
//STATIC		SINAL		TIPO	VARI¡ÅVEL				VALOR INICIAL		COMENT¡RIO
//-----------------------------------------------------------------------------------------------------------------------------------------------
							boolean	MX_PLSP_AUX				=0;					//Vari·vel auxiliar para funÁ„o plsp
							boolean	MX_PLSP_OUT				=0;					//Vari·vel de saida para funÁ„o plsp

	
/*===============================================================================================================================================
FLANCO POSITIVO
===============================================================================================================================================*/
//Esta funÁ„o verifica a transiÁ„o positiva de um bit
plsp(in)
{
	if(in)							//Verifica se a entrada foi acionada
	{
		if(!MX_PLSP_AUX)			//Verifica se j· estava acionada na ultima verificaÁ„o
		{
			MX_PLSP_OUT=TRUE;		//Se n„o, aciona saÌda e marcador auxiliar
			MX_PLSP_AUX=TRUE;		//
		}
		else MX_PLSP_OUT=FALSE;		//Se sim, desliga saÌda
	}
	else
	{
		MX_PLSP_OUT=FALSE;			//Se entrada n„o esta acionada, reseta saida e marcador auxiliar
		MX_PLSP_AUX=FALSE;			//
	}
	return(MX_PLSP_OUT);			//Retorna valor
}