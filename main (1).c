/*===============================================================================================================================================
DISPOSITIVO CORRETOR DE UMIDADE
PROGRAMADOR: LUCIANO BELLARMINO	

DESCRITIVO:


**HIST�RICO**
V1.0.00		Data: Julho de 2014
- Vers�o inicial.
===============================================================================================================================================*/
#include <18F4520.h>	//Inclui header do ucontrolador utilizado


/*===============================================================================================================================================
DEFINI��ES
===============================================================================================================================================*/
#define	IX_BTN_OP		PIN_D0			//Bot�o de acesso � tela de opera��o
#define	IX_BTN_REC		PIN_D1			//Bot�o de acesso � receita
#define	IX_BTN_PAR		PIN_D2			//Bot�o de acesso aos par�metros
#define	IX_BTN_NXT		PIN_D3			//Bot�o next
#define IX_BTN_REST		PIN_D4			//Bot�o acesso restrito
#define	IX_BTN_REC_SAVE	PIN_D5			//Bot�o salva receita

/*===============================================================================================================================================
CONFIGURA��ES
===============================================================================================================================================*/
#use delay (clock=8M)			//Informa � fun��o delay que o clock do oscilador � de 8MHz
#fuses INTRC					//Define oscilador interno


/*===============================================================================================================================================
BIBLIOTECAS
===============================================================================================================================================*/
#include <DisplayLDC.C>
#include <Transicao.h>
#include <Teclado_matricial.h>
#include <eeprom.h>

/*===============================================================================================================================================
DECLARA��O DAS VARI�VEIS
===============================================================================================================================================*/
//STATIC		SINAL		TIPO	VARI�VEL				VALOR INICIAL	E�PROM		COMENT�RIO
//-----------------------------------------------------------------------------------------------------------------------------------------------
				unsigned	int16	MW_SCREEN				=0;						//N�mero da tela a ser exibida
				unsigned	int16	MW_SCREEN_AUX			=0;						//Auxiliar
				unsigned	int8	MB_RECEITA				=0;						//N�mero da receita a ser executada
				unsigned	int8	MB_RECEITA_ED			=0;						//N�mero da receita a ser editada
				unsigned	int8	MB_ESTAGIO				=0;						//Etapa atual do processo
				unsigned	int16	MW_AGUA_EF				=0;						//Quantidade de �gua dosada
				unsigned	int8	MB_TEOR_AT				=0;						//Teor atual
				unsigned	int16	TON_IHM_ATUALIZA		=0;						//Temporizador para atualiza��o da IHM
				unsigned	int8	MB_TECLA_PRESS			=0;						//Tecla pressionada no teclado
				unsigned	int8	MB_TECLA_PRESS_AUX		=0;
							boolean	MX_PLSP_NXT				=0;						//Flanco positivo do bot�o next
//Parametros da receita--------------------------------------------------------------------------------------------------------------------------			
				unsigned	int8	MB_TEOR_SP				=0;						//Teor desejado
				unsigned	int16	MW_T_HOMO_INI_SP		=0;						//Tempo de homogeneiza��o inicial
				unsigned	int16	MW_TEOR_APROX_SP		=0;						//Teor de aproxima��o
				unsigned	int16	MW_TON_SP				=0;						//Tempo de valvula ligada
				unsigned	int16	MW_TOF_SP				=0;						//Tempo de v�lvula desligada
				unsigned	int16	MW_T_HOMO_SP			=0;						//Tempo de homogeneiza��o
				unsigned	int16	MW_TON_FINO_SP			=0;						//Tempo de valvula ligada ajuste fino
				unsigned	int16	MW_TOF_FINO_SP			=0;						//Tempo de v�lvula desligada ajuste fino
//Parametros-------------------------------------------------------------------------------------------------------------------------------------
				unsigned	int16	MW_GANHO				=0;				//000	Ganho do sensor
				unsigned	int16	MW_GANHO_AUX			=0;				//		Auxiliar para verificar altera��o
				unsigned	int16	MW_OFFSET				=0;				//002	Offset do sensor
				unsigned	int16	MW_OFFSET_AUX			=0;				//		Auxiliar para verificar altera��o
				unsigned	int16	MW_HIDR_REL				=0;				//004	Rela�ao de pulsos por litro do hidrometro
				unsigned	int16	MW_HIDR_REL_AUX			=0;				//		Auxiliar para verificar altera��o
				unsigned	int16	MW_METODO_LEITURA		=0;				//006	Metodo de leitura do sensor
				unsigned	int16	MW_METODO_LEITURA_AUX	=0;				//		Auxiliar para verificar altera��o
				unsigned	int16	MW_T_AMOSTRA			=0;				//008	Intervalo de amostragem
				unsigned	int16	MW_T_AMOSTRA_AUX		=0;				//		Auxiliar para verificar altera��o
				unsigned	int16	MW_FATOR_MEDIA			=0;				//010	Fator da media ponderada
				unsigned	int16	MW_FATOR_MEDIA_AUX		=0;				//		Auxiliar para verificar altera��o
				
/*===============================================================================================================================================
INTERRUP��ES
===============================================================================================================================================*/
//TIMER 0 - Esta interrup��o ser� executada conforme ajustado no setup_timer_0
#int_timer0
void timer_zero()
{
	set_timer0(15536);		//Seta Timer 0 em 15536 para que ocorra apenas 50000 contagens at� a proxima interrup��o que ocorre em 65536
							//fazendo com que decorra 0,1s
	
	TON_IHM_ATUALIZA++;		//Incrementa temporizador para atualiza��o da IHM
}
/*===============================================================================================================================================
FUN��ES
===============================================================================================================================================*/


/*===============================================================================================================================================
SUBROTINAS
===============================================================================================================================================*/
//ATUALIZA LCD DE ACORDO COM O NUMERO DA TELA QUE DEVE SER EXIBIDA
void IHM()
{
	if(input(IX_BTN_OP))	MW_SCREEN=1;	//ACESSO AOS MENUS PRINCIPAIS
	if(input(IX_BTN_REC))	MW_SCREEN=11;	//
	if(input(IX_BTN_PAR))	MW_SCREEN=101;	//
	
	MX_PLSP_NXT=plsp(input(IX_BTN_NXT));	//Verifica transi��o positiva do bot�o next
	
	MB_TECLA_PRESS=teclado3x4_scan();			//Move valor do scan do teclado para vari�vel utilizada no LCD
	if(MB_TECLA_PRESS_AUX!=teclado3x4_scan())	//Mant�m o valor da tecla pressionada na variavel por 1 scan
		MB_TECLA_PRESS_AUX=teclado3x4_scan();	//
	else MB_TECLA_PRESS=10;						//

	//EXIBI��O DA TELA DESEJADA
	switch(MW_SCREEN)
	{
		case 1:								//EXIBE TELA DE OPERA��O
		lcd_clear();						//Limpa LCD
		lcd_gotoxy(1,1);					//Monta primeira linha
		lcd_putc(" RC E VOL. TEOR");		//
		lcd_gotoxy(2,2);					//Monta segunda linha
		printf(lcd_putc, "%u",MB_RECEITA);	//
		lcd_gotoxy(5,2);					//
		printf(lcd_putc, "%u",MB_ESTAGIO);	//
		lcd_gotoxy(7,2);					//
		printf(lcd_putc, "%Lu",MW_AGUA_EF);	//
		lcd_putc("l");						//
		lcd_gotoxy(12,2);					//
		printf(lcd_putc, "%u",MB_TEOR_AT);	//
		lcd_putc("%");						//
		break;
//RECEITA============================================================================================		
		case 11:																	//EXIBE TELA DE SELE��O DA RECEITA A EXECUTAR
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("RECEITA");														//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "A EXECUTAR: [%02u]",MB_RECEITA);							//
		MB_RECEITA=teclado_3x4_valor(MB_TECLA_PRESS, MB_RECEITA, 2);				//Edi��o via teclado
		
		if(MX_PLSP_NXT)	MW_SCREEN++;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
//--------------		
		case 12:																	//EXIBE TELA DE SELE��O DA RECEITA A EDITAR
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("RECEITA");														//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "A EDITAR:   [%02u]",MB_RECEITA_ED);						//
		MB_RECEITA_ED=teclado_3x4_valor(MB_TECLA_PRESS, MB_RECEITA_ED, 2);			//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=13;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 13:																	//EXIBE TELA DE EDI��O DO TEOR DESEJADO
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("TEOR");															//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "DESEJADO:  [%02u",MB_TEOR_SP);							//
		lcd_putc("%]");																//
		MB_TEOR_SP=teclado_3x4_valor(MB_TECLA_PRESS, MB_TEOR_SP, 2);				//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=14;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 14:																	//EXIBE TELA DE EDI��O DO TEMPO DE HOMOGENEIZA��O INICIAL
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("T.HOMOGENEIZACAO");												//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "INICIAL:  [%03Lus]",MW_T_HOMO_INI_SP);					//
		MW_T_HOMO_INI_SP=teclado_3x4_valor(MB_TECLA_PRESS, MW_T_HOMO_INI_SP, 3);	//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=15;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;

//--------------				
		case 15:																	//EXIBE TELA DE EDI��O DO TEOR APROXIMADO
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("TEOR");															//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "APROXIMADO:[%02Lu",MW_TEOR_APROX_SP);						//
		lcd_putc("%]");																//
		MW_TEOR_APROX_SP=teclado_3x4_valor(MB_TECLA_PRESS, MW_TEOR_APROX_SP, 2);	//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=16;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 16:																	//EXIBE TELA DE EDI��O DO TEMPO DE V�LVULA LIGADA
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("TEMPO VALVULA");													//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "LIGADA:   [%03Lus]",MW_TON_SP);							//
		MW_TON_SP=teclado_3x4_valor(MB_TECLA_PRESS, MW_TON_SP, 3);					//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=17;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 17:																	//EXIBE TELA DE EDI��O DO TEMPO DE V�LVULA DESLIGADA
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("TEMPO VALVULA");													//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "DESLIGADA:[%03Lus]",MW_TOF_SP);							//
		MW_TOF_SP=teclado_3x4_valor(MB_TECLA_PRESS, MW_TOF_SP, 3);					//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=18;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;

//--------------		
		case 18:																	//EXIBE TELA DE EDI��O DO TEMPO DE HOMOGENEIZA��O
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("T.HOMOGENEIZACAO");												//
		lcd_gotoxy(11,2);															//Monta segunda linha
		printf(lcd_putc, "[%03Lus]",MW_T_HOMO_SP);									//
		MW_T_HOMO_SP=teclado_3x4_valor(MB_TECLA_PRESS, MW_T_HOMO_SP, 3);			//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=19;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 19:																	//EXIBE TELA DE EDI��O DO TEMPO DE HOMOGENEIZA��O
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("TEMPO VALVULA F");												//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "LIGADA:   [%03Lus]",MW_TON_FINO_SP);						//
		MW_TON_FINO_SP=teclado_3x4_valor(MB_TECLA_PRESS, MW_TON_FINO_SP, 3);		//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=20;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 20:																	//EXIBE TELA DE EDI��O DO TEMPO DE HOMOGENEIZA��O
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("TEMPO VALVULA F");												//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "DESLIGADA:[%03Lus]",MW_TOF_FINO_SP);						//
		MW_TOF_FINO_SP=teclado_3x4_valor(MB_TECLA_PRESS, MW_TOF_FINO_SP, 3);		//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=11;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
							
//PARAMETROS================================================================================================			
		case 101:																	//EXIBE TELA DE EDI��O DO GANHO
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("----------------");												//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "GANHO:    [%04Lu]",MW_GANHO);								//
		MW_GANHO=teclado_3x4_valor(MB_TECLA_PRESS, MW_GANHO, 4);					//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=102;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 102:																	//EXIBE TELA DE EDI��O DO OFFSET
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("----------------");												//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "OFFSET:   [%04Lu]",MW_OFFSET);							//
		MW_OFFSET=teclado_3x4_valor(MB_TECLA_PRESS, MW_OFFSET, 4);					//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=103;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 103:																	//EXIBE TELA DE EDI��O DA RELA��O DE PULSOS DO HIDROMETRO
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("HIDROMETRO");														//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "REL.: [%05.2Lwp/l]",MW_HIDR_REL);							//
		MW_HIDR_REL=teclado_3x4_valor(MB_TECLA_PRESS, MW_HIDR_REL, 4);				//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=104;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 104:																	//EXIBE TELA DE EDI��O DO METODO DE LEITURA
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("METODO DE");														//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "LEITURA:     [%01Lu]",MW_METODO_LEITURA);					//
		MW_METODO_LEITURA=teclado_3x4_valor(MB_TECLA_PRESS, MW_METODO_LEITURA, 1);	//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=105;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 105:																	//EXIBE TELA DE EDI��O DO INTERVALO DA AMOSTRA
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("INTERVALO");														//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "AMOSTRA:[%04Lums]",MW_T_AMOSTRA);							//
		MW_T_AMOSTRA=teclado_3x4_valor(MB_TECLA_PRESS, MW_T_AMOSTRA, 4);			//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=106;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
//--------------		
		case 106:																	//EXIBE TELA DE EDI��O DA MEDIA
		lcd_clear();																//Limpa LCD
		lcd_gotoxy(1,1);															//Monta primeira linha
		lcd_putc("FATOR MEDIA");													//
		lcd_gotoxy(1,2);															//Monta segunda linha
		printf(lcd_putc, "PONDERADA: [%03Lu]",MW_FATOR_MEDIA);						//
		MW_FATOR_MEDIA=teclado_3x4_valor(MB_TECLA_PRESS, MW_FATOR_MEDIA, 3);		//Edi��o via teclado

		if(MX_PLSP_NXT)	MW_SCREEN=101;									//Verifica se bot�o de avan�o foi pressionado. Se sim, chama proxima tela	
		break;
		
	}
}

//SALVA VALORES RETENTIVOS NA EEPROM NA TRANSI��O DE TELAS CASO TENHAM SIDO ALTERADOS
void EEPROM()
{
	if(MW_SCREEN_AUX!=MW_SCREEN)			//Verifica se houve transi��o na tela exibida. Se houver, verifica altera��o nos valores
	{										//
		MW_SCREEN_AUX=MW_SCREEN;			//
		
		if(MW_GANHO_AUX!=MW_GANHO)						//Verifica se houve altera��o no parametro restrito GANHO
		{												//
			MW_GANHO_AUX=MW_GANHO;						//
			write_eeprom_word(0, MW_GANHO);				//Atualiza valor na EEPROM
		}
		if(MW_OFFSET_AUX!=MW_OFFSET)					//Verifica se houve altera��o no parametro restrito OFFSET
		{												//
			MW_OFFSET_AUX=MW_OFFSET;					//
			write_eeprom_word(2, MW_OFFSET);			//Atualiza valor na EEPROM
		}
		if(MW_HIDR_REL_AUX!=MW_HIDR_REL)				//Verifica se houve altera��o no parametro restrito RELA��O HIDROMETRO
		{												//
			MW_HIDR_REL_AUX=MW_HIDR_REL;				//
			write_eeprom_word(4, MW_HIDR_REL);			//Atualiza valor na EEPROM
		}
		if(MW_METODO_LEITURA_AUX!=MW_METODO_LEITURA)	//Verifica se houve altera��o no parametro restrito METODO DE LEITURA
		{												//
			MW_METODO_LEITURA_AUX=MW_METODO_LEITURA;	//
			write_eeprom_word(6, MW_METODO_LEITURA);	//Atualiza valor na EEPROM
		}
		if(MW_T_AMOSTRA_AUX!=MW_T_AMOSTRA)				//Verifica se houve altera��o no parametro restrito INTERVALO DE AMOSTRAGEM
		{												//
			MW_T_AMOSTRA_AUX=MW_T_AMOSTRA;				//
			write_eeprom_word(8, MW_T_AMOSTRA);			//Atualiza valor na EEPROM
		}
		if(MW_FATOR_MEDIA_AUX!=MW_FATOR_MEDIA)			//Verifica se houve altera��o no parametro restrito FATOR MEDIA CAMINHANTE
		{												//
			MW_FATOR_MEDIA_AUX=MW_FATOR_MEDIA;			//
			write_eeprom_word(8, MW_FATOR_MEDIA);		//Atualiza valor na EEPROM
		}								
	}	
}	
/*===============================================================================================================================================
ROTINA PRINCIPAL
===============================================================================================================================================*/
void main()
{
	//CONFIGURA��O DO OSCILADOR
	setup_oscillator(OSC_8MHZ);		//Determina que o clock do oscilador interno ser� de 8MHz

	//CONFIGURA��O DO TIMER 0 E DA INTERRUP��O POR TEMPO
	enable_interrupts(GLOBAL);								//Habilita interrup��es globais
	enable_interrupts(INT_TIMER0);							//Habilita interrup��o pelo TIMER 0
	setup_timer_0 (RTCC_DIV_4|RTCC_INTERNAL);				//Prescaler:4 / Clock Interno / 16bits	(INC a cada 2us / Interrup��o a cada 0,1s
	set_timer0(15536); 										//Seta Timer 0 em 15536 para que ocorra apenas 50000 contagens at� a proxima
															//interrup��o que ocorre em 65536 e assim esta ocorra em 0,1s como desejado	
	
	//CONFIGURA��O DO TIMER 1
	setup_timer_1 (T1_DIV_BY_4|T1_INTERNAL);				//Prescaler:4 / Clock Interno / 16bits	(INC a cada 2us / Interrup��o a cada 0,1s
	
	//CARREGA VALORES RETENTIVOS DA EEPROM
	MW_GANHO=read_eeprom_word(0);
	MW_GANHO_AUX=MW_GANHO;
	MW_OFFSET=read_eeprom_word(2);
	MW_OFFSET_AUX=MW_OFFSET;
	MW_HIDR_REL=read_eeprom_word(4);
	MW_HIDR_REL_AUX=MW_HIDR_REL;
	MW_METODO_LEITURA=read_eeprom_word(6);
	MW_METODO_LEITURA_AUX=MW_METODO_LEITURA;
	MW_T_AMOSTRA=read_eeprom_word(8);
	MW_T_AMOSTRA_AUX=MW_T_AMOSTRA;
	MW_FATOR_MEDIA=read_eeprom_word(10);
	MW_FATOR_MEDIA_AUX=MW_FATOR_MEDIA;
	
	//INICIALIZA��O DO LCD
	lcd_init();							//Inicializa LCD
	
	//MENSAGEM INICIAL	
	lcd_gotoxy(1,1);					//Exibe mensagem inicial
	lcd_putc("    CORRETOR");			//
	lcd_gotoxy(1,2);					//
	lcd_putc("   DE UMIDADE");			//
	
	delay_ms(1000);						//Aguarda delay
	
	lcd_clear();						//Limpa LCD
	lcd_gotoxy(9,2);					//Exibe vers�o do software
	lcd_putc("V1.0.00b");				//	
	
	delay_ms(1000);						//Aguarda delay
	
	MW_SCREEN=1;						//Carrega tela inicial como alvo
	
	while(true)		//La�o infinito pra execu��o ciclica do programa
	{	
		//CHAMADA DA SUBROTINA DE CONTROLE DA IHM
		if(TON_IHM_ATUALIZA>=1)	//Chama controle da IHM a cada 100ms
		{
			TON_IHM_ATUALIZA=0;	//Zera temporizador de chamada do controle da IHM
			IHM();
			EEPROM();
		}	
	}
}