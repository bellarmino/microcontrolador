/*===============================================================================================================================================
DECLARA«√O DAS VARI¡ÅVEIS
===============================================================================================================================================*/
//STATIC		SINAL		TIPO	VARI¡ÅVEL				VALOR INICIAL		COMENT¡ÅRIO
//-----------------------------------------------------------------------------------------------------------------------------------------------
							boolean	teclado3x4_on			=0;					//Indica que existe tecla pressionada no teclado 3x4
							
				unsigned	int32	MWL_OUT					=0;					//SaÌda da funÁ„o de leitura da entrada de valor
				unsigned	int32	MBL_DG1					=0;					//Vari·vel para armazenamento do primeiro digito (LSB)
				unsigned	int32	MBL_DG2					=0;					//Vari·vel para armazenamento do segundo digito
				unsigned	int32	MBL_DG3					=0;					//Vari·vel para armazenamento do terceiro digito
				unsigned	int32	MBL_DG4					=0;					//Vari·vel para armazenamento do quarto digito
				unsigned	int32	MBL_DG5					=0;					//Vari·vel para armazenamento do quinto digito (MSB)


/*===============================================================================================================================================
TECLADO 3X4
===============================================================================================================================================*/
//SCAN
//Esta funÁao retorna o valor da tecla pressionada em um teclado matricial 3x4
teclado3x4_scan()
{
		#define CL1	PIN_C0
		#define CL2	PIN_C1
		#define CL3	PIN_C2
		#define LN1	PIN_C3
		#define LN2	PIN_C4
		#define LN3	PIN_C5
		#define LN4	PIN_C6		

//Executa varredura da primeira coluna	
	output_high(CL1);
	output_low(CL2);
	output_low(CL3);
	if(input(LN1)) 
	{
		teclado3x4_on=TRUE;
		return(1);
	}	
	if(input(LN2))
	{
		teclado3x4_on=TRUE;
		return(4);
	}	
	if(input(LN3))
	{
		teclado3x4_on=TRUE;
		return(7);
	}	
//Executa varredura da segunda coluna
	output_low(CL1);
	output_high(CL2);
	output_low(CL3);
	if(input(LN1))
	{
		teclado3x4_on=TRUE;
		return(2);
	}	
	if(input(LN2))
	{
		teclado3x4_on=TRUE;
		return(5);
	}	
	if(input(LN3))
	{
		teclado3x4_on=TRUE;
		return(8);
	}	
	if(input(LN4))
	{
		teclado3x4_on=TRUE;
		return(0);
	}	
//Executa varredura da terceira coluna	
	output_low(CL1);
	output_low(CL2);
	output_high(CL3);
	if(input(LN1))
	{
		teclado3x4_on=TRUE;
		return(3);
	}	
	if(input(LN2))
	{
		teclado3x4_on=TRUE;
		return(6);
	}	
	if(input(LN3))
	{
		teclado3x4_on=TRUE;
		return(9);
	}
	output_low(CL1);
	output_low(CL2);
	output_low(CL3);	
	return(10);
	
	teclado3x4_on=FALSE;		//Reseta indicaÁ„o de que existe tecla pressionada
}

int32 teclado_3x4_valor(tecla,int32 valor, casas)
{
	MWL_OUT=valor;	//Iguala a saÌda da funÁ„o ao valor da entrada para evitar transferencia de dados entre vari·veis por lixo na saÌda
	if(tecla<10)
		{
			MBL_DG5=valor/10000;		//Separa valor atual em digitos
			MBL_DG4=(valor%10000)/1000;	//
			MBL_DG3=(valor%1000)/100;	//
			MBL_DG2=(valor%100)/10;		//
			MBL_DG1=valor%10;			//
		
			MBL_DG5=MBL_DG4;			//Rotaciona digitos do valor atual
			MBL_DG4=MBL_DG3;			//
			MBL_DG3=MBL_DG2;			//
			MBL_DG2=MBL_DG1;			//
			MBL_DG1=tecla;				//Insere valor do scan do teclado como digito 1 (LSB)
			
			switch(casas)				//Zera casas n„o desejadas
			{
				case 1:
				MBL_DG5=0;
				MBL_DG4=0;
				MBL_DG3=0;
				MBL_DG2=0;
				
				case 2:
				MBL_DG5=0;
				MBL_DG4=0;
				MBL_DG3=0;
				
				case 3:
				MBL_DG5=0;
				MBL_DG4=0;
				
				case 4:
				MBL_DG5=0;
			}
			
			MWL_OUT=MBL_DG1+(MBL_DG2*10);		//Une digitos em uma unica vari·vel
			MWL_OUT=MWL_OUT+(MBL_DG3*100);		//
			MWL_OUT=MWL_OUT+(MBL_DG4*1000);		//
			MWL_OUT=MWL_OUT+(MBL_DG5*10000);	//
		}
	return(MWL_OUT);	
}	