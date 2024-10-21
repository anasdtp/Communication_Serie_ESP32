#ifndef _Communication_LIB
#define _Communication_LIB
#include <Arduino.h>
#include <BluetoothSerial.h>

//Choisir ses IDs de messages 

#define ID_CMD_GENERAL 0xA0 
#define ID_CMD_XYT 0xA1 //Exemple de commande

#define ID_ACK_GENERAL 0xC0 //Ack pour tous le reste

#define ID_REPEAT_REQUEST 0xD0
#define ID_POS_XYT 0xD1

typedef struct Message{
    uint8_t id;
    uint8_t len;
    uint8_t *data;
    uint8_t checksum;
}Message;
#define SIZE_FIFO 32 //au maximum 150 du fait du type char

class Communication
{
public:
    Communication(/* args */);

    void begin(HardwareSerial *srl = &Serial, long baud = 921600, String nameBT = "ESP32_COM");
    void end();


    int getFIFO_Ecriture(){
        return FIFO_ecriture;
    }
    Message getMsg(int curseur){
        return rxMsg[curseur];
    }

    void RxManage();

    void sendMsg(Message &txMsg);
    void sendMsg(uint8_t id, uint8_t len, uint8_t *data);
    void sendMsg(uint8_t id);
    void sendMsg(uint8_t id, uint8_t octet);
    void sendMsg(uint8_t id, uint8_t octet1, uint8_t octet2);
    void sendMsg(uint8_t id, uint16_t nb);
    void sendMsg(uint8_t id, uint16_t a,  uint16_t b);
    void sendMsg(uint8_t id, uint16_t a,  uint16_t b, uint16_t c);
    void sendMsg(uint8_t id, uint32_t nb);

    void printMessage(Message msg);

    
    bool getRandomRequest(bool afterCheck = false){
      if(_randomRequest){
        _randomRequest = afterCheck;
        return true;
      }
      return false;
    }
    void setRandomRequest(bool value){
      _randomRequest = value;
    }

    bool getRepeatRequest(bool afterCheck = false){
      if(_repeatRequest){
        _repeatRequest = afterCheck;
        return true;
      }
      return false;
    }
    void setRepeatRequest(bool value){
      _repeatRequest = value;
    }

private:
    HardwareSerial *_serial;
    BluetoothSerial *_serialBT;

    Message rxMsg[SIZE_FIFO]; // FIFO de messages reçus
    int FIFO_ecriture;// FIFO_ecriture est l'indice de l'écriture

    //Tous les flags doivent etre declarés ici :
    bool _repeatRequest, _randomRequest;

    // État de la réception
    enum StateRx{
      WAITING_HEADER, //FF
      RECEIVING_ID,   //ID
      RECEIVING_LEN,  //LEN
      RECEIVING_DATA, //DATA
      RECEIVING_CHECKSUM, //CHECKSUM
      WAITING_FOOTER  //FF
    };

    void receiveByte(uint8_t byte);

    void onReceiveFunction(void); //Fonction d'interruption de réception de données. On reçoit un octet à la fois

    static Communication* instance;  // Static pointer to the current instance, to be used in the static bluetooth callback function
    static void onReceiveFunctionBTStatic(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
    void onReceiveFunctionBT(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
};

#endif // _Communication_LIB