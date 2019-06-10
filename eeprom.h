/*===============================================================================================================================================
DECLARA«√O DAS VARI¡ÅVEIS
===============================================================================================================================================*/
//STATIC		SINAL		TIPO	VARI¡ÅVEL				VALOR INICIAL		COMENT¡RIO
//-----------------------------------------------------------------------------------------------------------------------------------------------


/*===============================================================================================================================================
SALVAR WORD NA EEPROM
===============================================================================================================================================*/
void write_eeprom_word(endereco,int16 valor)
{
	write_eeprom(endereco, valor/256);		//Salva parte alta na eeprom (o quociente da divis„o por 256 retorna a parte alta da word)
	write_eeprom(endereco+1, valor%256);	//Salva parte baixa na eeprom (o resto da divis„o por 256 retorna a parte baixa da word)
	
	return;
}			

/*===============================================================================================================================================
LER WORD DA EEPROM
===============================================================================================================================================*/
int16 read_eeprom_word(endereco)
{
	return(read_eeprom(endereco)*256+read_eeprom(endereco+1));	//Para retornar o valor de uma word da eeprom (2 bytes consecutivos, le-se a parte alta
													//multiplica-se por 256 e ent„o soma-se a parte baixa
}