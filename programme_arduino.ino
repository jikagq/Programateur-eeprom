/*
 * Programing EEPROM with Arduino nano and Labview
 * TPS dut geii 2016
 * For S2M
 * 
 */
 
#include <SPI.h>
//déclaration des broches de commmunication spi pour le uno/nano
#define DATAIN 12//miso
#define DATAOUT 11//mosi
#define CLK 13//sck
#define CHIPSELECT 10//cs

//Instructions EEPROM
#define READ 3  
#define WRITE 2 
#define WRDI  4
#define WREN 6
#define RDSR  5
#define WRSR 1

/**
les 14 choix 
1=board serial number
2=code article
3=test sonde mobile
4=test synor
5=test ict
6=test fct1
7=test ess
8=test fct2
9=version soft1
10=version soft2
11=commentaire1
12=version soft3
13=mapping
14=commentaire2
**/
//variable pour la led
const int ledPin =  5;// alias de la broche de la led
int ledState = LOW;// etat de la led
long previousMillis = 0;//temps écoulé depuis le dernier changment d'etat 
long interval = 1000;  //intervalle

int compteur=0;//
char eeprom_output_data=0;//données envoyer par l'eeprom
String eeprom_input_data=""; //données envoyer par l'arduino
String instructionlabview="";//variable de reception read ou write
String datain=""; //buffer de lecture

void setup() {//initialisation des broches pour le spi 
Serial.begin(9600);
pinMode(ledPin,OUTPUT);//led
pinMode(DATAIN, INPUT);
pinMode(DATAOUT, OUTPUT);
pinMode(CLK,OUTPUT);
pinMode(CHIPSELECT,OUTPUT);
digitalWrite(CHIPSELECT,HIGH); 
SPI.beginTransaction(SPISettings(5000000, MSBFIRST, SPI_MODE0));
}


void loop() {//boucle infinie
unsigned long currentMillis = millis();//stocke la valeur courante de la fonction millis()
if(currentMillis - previousMillis > interval) {
  previousMillis = currentMillis;// mémorise la valeur de la fonction millis()   
  if (ledState == LOW)// inverse la variable d''état de la LED
    ledState = HIGH;
  else
    ledState = LOW;
  digitalWrite(ledPin, ledState); // modifie l'état de la LED
}

instructionlabview=ecoutelabview();//récupere les instructions read ou write 

if(instructionlabview=="READ")
  {
    //Serial.println("instruction read reçu");
    //delay(100);
    String adressedebut=ecoutelabview();//recup adresse debut
    int adressedebutint =atoi(adressedebut.c_str());//conversion
    delay(50);
    String adressefin=ecoutelabview();//recup adresse fin
    int adressedefinint =atoi(adressefin.c_str());//conversion
    delay(50);
    Serial.println(lectureadresse(adressedebutint,adressedefinint));//appel de la fonction lecture
    datain="";//raz du buffer
    delay(50);

  }


/////////////////////////////////////////////
if(instructionlabview=="WRITE")
  {
    //Serial.println("instruction write reçu");
    delay(100);
    
    eeprom_input_data=ecoutelabview();//reception données
    delay(100);

    String stringadresse=ecoutelabview();//reception adresse debut  
    int adresse = atoi(stringadresse.c_str());//conversion en entier
    delay(100);

    String stringadressefin=ecoutelabview();//reception adresse de fin +1
    int adressefin = atoi(stringadressefin.c_str());//conversion en entier
    delay(100);
    
    ecritureavecadressse(eeprom_input_data,adresse,adressefin);//appel de la fonction écriture
    datain="";//raz du buffer
    delay(100);

  }

delay(50);
}//fin loop


//FONCTION LECTURE byte
byte lecture_eeprom(byte eeprom_adresse)
{
   digitalWrite(5,HIGH);
   int datarecu;
   digitalWrite(CHIPSELECT,LOW);//cs=0
   SPI.transfer(READ);//envoi de read en spi 
   SPI.transfer(eeprom_adresse);//envoi de l'adresse byte en spi   
   datarecu = SPI.transfer(0);//reception du byte 
   digitalWrite(CHIPSELECT,HIGH);//cs=1
   return datarecu;//revoi de la donnée
}

//FONCTION ECRITURE byte
void ecriturebyte(byte eeprom_adresse, byte data_a_envoyer)
{
  digitalWrite(5,HIGH);
  digitalWrite(CHIPSELECT,LOW);//cs=0
  SPI.transfer(WREN);//envoi de wren en spi (write enable) 
  digitalWrite(CHIPSELECT,HIGH);
  digitalWrite(CHIPSELECT,LOW);
  SPI.transfer(WRITE);//envoi de write en spi (write enable) 
  SPI.transfer(eeprom_adresse);////envoi de l'adresse du byte en spi
  SPI.transfer(data_a_envoyer);////envoi du byte en spi
  delay(10);
  digitalWrite(CHIPSELECT,HIGH);
}

//ECOUTE LABVIEW
String ecoutelabview()
{
char incomingByte = 0;//caractére reçu
String incomingval="";//buffer
String instructionbrute="";//valeur   
while(Serial.available() > 0)//tant qu'il y a quelque chose sur le port
{ 
  if(Serial.available() > 0) //si il y a quelque chose sur le port 
         { 
          incomingByte = Serial.read();//recupération d'un caractere et stockage dans une variable
          if(incomingByte == 13)//si c'est un retour chariot(cad fin dfe la trame labview)
           {
            instructionbrute=incomingval;//stockage du buffer dans une nouvelle variable
            delay(50);
            incomingval="";//vidange buffer
            break;//sortie de boucle
            }
           else//si ce n'est pas un retour chariot
            {
             incomingval=incomingval+incomingByte;//ajout du caractére reçu dans le buffer
             delay(1);
            }           
        }
 }        
return instructionbrute;//lorsque la trame est fini
}

//LECTURE AVEC ADRESSE
String lectureadresse(int adressedebut, int adressefin)
{
for(adressedebut;adressedebut<adressefin;adressedebut++)
  {

    eeprom_output_data = lecture_eeprom(adressedebut);
    datain=datain+eeprom_output_data;
    delay(50);

  }
eeprom_output_data=0;
return datain;
}

//ECRITURE AVEC ADRESSE
void ecritureavecadressse(String data, int adressedebut, int adressefin)
{
for(adressedebut;adressedebut<adressefin;adressedebut++)
  {
  byte unseulchar = data.charAt(compteur);//recuperation d'un seul byte 
  ecriturebyte(adressedebut,unseulchar);
  compteur++;
  delay(100);
  }
compteur=0;

}




