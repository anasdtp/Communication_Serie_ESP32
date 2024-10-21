#include <Arduino.h>
#include <Communication.h>

Communication com;

void RxManage();
void robot_goto(uint16_t x, uint16_t y, uint16_t theta);

void setup() {
  com.begin(&Serial, 921600, "ESP32_COM");

  com.sendMsg(ID_CMD_GENERAL);

}

void loop() {
  RxManage();
}

void RxManage(){
    static signed char FIFO_lecture = 0, FIFO_occupation = 0, FIFO_max_occupation = 0;

    FIFO_occupation = com.getFIFO_Ecriture() - FIFO_lecture;
    if(FIFO_occupation<0){FIFO_occupation=FIFO_occupation+SIZE_FIFO;}
    if(FIFO_max_occupation<FIFO_occupation){FIFO_max_occupation=FIFO_occupation;}
    if(!FIFO_occupation){return;}

    //Alors il y a un nouveau message en attente de traitement
    // printMessage(rxMsg[FIFO_lecture]);
    Message rxMsg = com.getMsg(FIFO_lecture);
    
    switch (rxMsg.id)
    {
        case ID_CMD_GENERAL:{
            com.setRandomRequest(true);
            com.sendMsg(ID_ACK_GENERAL, (uint8_t)ID_CMD_GENERAL);//Un ack pour dire qu'on a bien reçu la commande, et en specifiant la commande dans la data
        }break;

        case ID_REPEAT_REQUEST:{
            com.setRepeatRequest(true);
            com.sendMsg(ID_ACK_GENERAL, (uint8_t)ID_REPEAT_REQUEST);
        }break;

        case ID_CMD_XYT:{//Par exemple
            uint16_t x =    (uint16_t)(rxMsg.data[0] | (rxMsg.data[1] << 8));
            uint16_t y =    (uint16_t)(rxMsg.data[2] | (rxMsg.data[3] << 8));
            uint16_t theta =(uint16_t)(rxMsg.data[4] | (rxMsg.data[5] << 8));
            com.sendMsg(ID_ACK_GENERAL, (uint8_t)ID_CMD_XYT, (uint8_t)0);//0 pour dire qu'on a reçu, par exemple

            robot_goto(x, y, theta);

            com.sendMsg(ID_ACK_GENERAL, (uint8_t)ID_CMD_XYT, (uint8_t)1);//1 pour dire qu'on a fini de le faire

            com.sendMsg(ID_POS_XYT, x, y, theta);
        }break;

        default:
            com.sendMsg(ID_ACK_GENERAL);
            break;
    }

    FIFO_lecture = (FIFO_lecture + 1) % SIZE_FIFO;//On passe au message suivant
}

void robot_goto(uint16_t x, uint16_t y, uint16_t theta){
    //Fonction de déplacement du robot, par exemple
}