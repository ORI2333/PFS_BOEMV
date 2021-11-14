/*
 * Copyright (c) 2020 HiHope Community.
 * Description: nfc demo
 * Author: HiSpark Product Team.
 * Create: 2020-5-20
 */
#include <hi_i2c.h>
#include <hi_types_base.h>
#include <c081_nfc.h>
#include <hi_io.h>
#include <hi_time.h>
#include <hi_task.h>
#include <hi_mux.h>
#include <hi_watchdog.h>
#include <hi_early_debug.h>
#include <string.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include <hi_watchdog.h>
#include <hi_isr.h>
#include <ssd1306_oled.h>
#include <app_demo_multi_sample.h>

T4T_FILE current_file;

hi_u8 capability_container[15] =
{   0x00, 0x0F,        //CCLEN  
    0x20,              //Mapping Version 
    0x00, 0xF6,        //MLe If F6 is written as FF, it will be divided into frames if it exceeds 256 bytes, but if F6 is written, it will not be divided into frames
    0x00, 0xF6,        //MLc If F6 is written as FF, it will be divided into frames if it exceeds 256 bytes, but if F6 is written, it will not be divided into frames
    0x04,              //NDEF message format 05 is private
    0x06,              //NDEF Message length
    0xE1, 0x04,        //NDEF FILE ID      File identifier for NDEF
    0x03, 0x84,        //NDEF最大长度
    0x00,              //Read Access           readable
    0x00               //Write Access          writeable
};             

hi_u8 ndef_file[1024] = { 
#ifdef HUAWEI_COM    
    /*http://wwww.huawei.com*/
    0x00,0x0F,                    
    0xD1,0x01,0x0B,0x55,
    0x01,0x68,0x75,0x61,
    0x77,0x65,0x69,0x2E,
    0x63,0x6F,0x6D,
#endif    
/*wechat*/
#ifdef  NFC_TAG_WECHAT
0x00,0x20,
0xd4, 0x0f,0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f,
0x69, 0x64,0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
0x6b, 0x67,0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x65,
0x6e, 0x63,0x65, 0x6e, 0x74, 0x2e, 0x6d, 0x6d,
#endif
};

hi_u8 fm327_fifo[1024];
hi_u8 irq_data_in = 0;//Identification of non connected data receiving terminal
hi_u8 irq_txdone = 0;
hi_u8 rfLen;
hi_u8 irq_rxdone = 0;
hi_u8 irq_data_wl =0;
hi_u8 FlagFirstFrame = 0; //First frame identification of card

hi_u32 g_c08i_nfc_demo_task_id = 0;
hi_u32 g_nfc_display_task_id = 0;
extern hi_u32 g_mux_id;
hi_u8 read_reg =0;

/**
	@berf i2c read
	@param hi_u8 reg_high_8bit_cmd:Transmit register value 8 bits high
	@param hi_u8 reg_low_8bit_cmd:Transmit register value low 8 bits
	@param hi_u8* recv_data:Receive data buff
	@param hi_u8 send_len:Sending data length
	@param hi_u8 read_len:Length of received data
*/
hi_u32 write_read(hi_u8 reg_high_8bit_cmd, hi_u8 reg_low_8bit_cmd, hi_u8* recv_data, hi_u8 send_len, hi_u8 read_len)
{
    hi_i2c_idx id = 0;
    hi_u32 status =0;
    hi_i2c_data co8i_nfc_i2c_read_data ={0};
    hi_i2c_data c081nfc_i2c_write_cmd_addr ={0};
    hi_u8 _send_user_cmd[2] = {reg_high_8bit_cmd, reg_low_8bit_cmd};

    memset(recv_data, 0x0, sizeof(recv_data));
    memset(&co8i_nfc_i2c_read_data, 0x0, sizeof(hi_i2c_data));

    c081nfc_i2c_write_cmd_addr.send_buf = _send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = send_len;

    co8i_nfc_i2c_read_data.receive_buf = recv_data;
    co8i_nfc_i2c_read_data.receive_len = read_len;

    read_reg = NFC_CLEAN;//Eliminate stop signal

    status = hi_i2c_write(id, C081_NFC_ADDR&0xFE, &c081nfc_i2c_write_cmd_addr);
    status = hi_i2c_read(id, C081_NFC_ADDR|I2C_RD, &co8i_nfc_i2c_read_data);

    return HI_ERR_SUCCESS;
}

/**
	@berf co8i write command,The interface writes EEPROM to change the chip configuration
	@param hi_u8 reg_high_8bit_cmd:Write command high 8 bits
	@param hi_u8 reg_low_8bit_cmd:Write command low 8 bits
	@param hi_u8* data_buff:send data
	@param hi_u8 len:Sending data length
*/
hi_u32 c08i_nfc_i2c_write( hi_u8 reg_high_8bit_cmd, hi_u8 reg_low_8bit_cmd, hi_u8* data_buff, hi_u8 len)
{
    hi_u32 status =0;
    hi_i2c_idx id = 0;
    hi_i2c_data c081nfc_i2c_write_cmd_addr ={0};
    hi_u8 _send_user_cmd[64] = {reg_high_8bit_cmd, reg_low_8bit_cmd};

    c081nfc_i2c_write_cmd_addr.send_buf = _send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = 2+len;
    for (hi_s32 i=0; i<len; i++) {
        _send_user_cmd[2+i] = *(data_buff+i);
    }
    status = hi_i2c_write(id, C081_NFC_ADDR&0xFE, &c081nfc_i2c_write_cmd_addr);

    return HI_ERR_SUCCESS;
}

/** 
	@berf Write register
	@param hi_u8 reg_high_8bit_cmd:Send data high 8 bits
	@param hi_u8 reg_low_8bit_cmd:Send data low 8 bits
	@param hi_u8 data_buff:Send data buff
*/
hi_u32 write_fifo_reg( hi_u8 reg_high_8bit_cmd, hi_u8 reg_low_8bit_cmd, hi_u8 data_buff)
{
    hi_u32 status =0;
    hi_i2c_idx id = 0;
    hi_i2c_data c081nfc_i2c_write_cmd_addr ={0};
    hi_u8 _send_user_cmd[3] = {reg_high_8bit_cmd, reg_low_8bit_cmd, data_buff};

    c081nfc_i2c_write_cmd_addr.send_buf = _send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = 3;

    status = hi_i2c_write(id, C081_NFC_ADDR&0xFE, &c081nfc_i2c_write_cmd_addr);

    return HI_ERR_SUCCESS;
}

/**
	@berf write fifo data
	@param hi_u8* data_buff:Send data buff
	@param hi_u8 len: Sending data length
*/
hi_u32 write_fifo_data( hi_u8* data_buff, hi_u8 len)
{
    hi_u32 status =0;
    hi_i2c_idx id = 0;
    hi_i2c_data c081nfc_i2c_write_cmd_addr ={0};
    hi_u8 _send_user_cmd[128] = {0};

    memset(_send_user_cmd, 0x0, sizeof(_send_user_cmd));

    _send_user_cmd[0] = 0xff;
    _send_user_cmd[1] =0xf0;

    for (int i=0; i<len; i++) {
        _send_user_cmd[2+i] = *(data_buff+i);
    }
    c081nfc_i2c_write_cmd_addr.send_buf = _send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = 2+len;
    status = hi_i2c_write(id, C081_NFC_ADDR&0xFE, &c081nfc_i2c_write_cmd_addr);

    return HI_ERR_SUCCESS;
}

/*8
	@berf EEPROM page write
	@param hi_u8 *pBuffer: Send data buff
	@param hi_u16 WriteAddr:Send register address
	@param hi_u8 datalen:Sending data length
*/
hi_void eep_write_page(hi_u8 *pBuffer, hi_u16 WriteAddr, hi_u8 datalen)
{
    hi_u32 status;
    status = c08i_nfc_i2c_write((hi_u8)((WriteAddr & 0xFF00) >> 8), (hi_u8)(WriteAddr & 0x00FF), pBuffer, datalen);
	hi_udelay(10000);//The delay time must be 10ms
    hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_9, HI_GPIO_VALUE1);
    printf("----- eep_write_page %d : %s! -----\r\n\n", __LINE__,status == HI_ERR_SUCCESS ? "SUCCESS!" : "FAILURE!");
}

/**
	@berf write EEPROM
	@param hi_u16 addr:eeprom address
	@param hi_u32 len:Write data length
	@param hi_u8 *wbuf:write data buff
*/
hi_void fm11_write_eep(hi_u16 addr,hi_u32 len,hi_u8 *wbuf)
{
	hi_u8 offset;
    if (addr < FM11_E2_USER_ADDR || addr >= FM11_E2_MANUF_ADDR) {
        return;
    }     
    if (addr % FM11_E2_BLOCK_SIZE) {
        offset = FM11_E2_BLOCK_SIZE - (addr % FM11_E2_BLOCK_SIZE);
        if (len > offset) {
            eep_write_page(wbuf,addr,offset);
            
            addr += offset;
            wbuf += offset;
            len -= offset;
        } else {
            eep_write_page(wbuf,addr,len);
            len = 0;
        }
     }
    while (len) {
        if (len >= FM11_E2_BLOCK_SIZE) {
            eep_write_page(wbuf,addr,FM11_E2_BLOCK_SIZE);
            addr += FM11_E2_BLOCK_SIZE;
            wbuf += FM11_E2_BLOCK_SIZE;
            len -= FM11_E2_BLOCK_SIZE;
        } else {
            eep_write_page(wbuf,addr,len);
            len = 0;
        }
    }
}

/*NFC chip configuration, usually do not call NFC init*/
hi_void nfc_init(hi_void)
{
 	// 	uint8_t wbuf[5]={0x05,0x72,0xF7,0x60,0x02};   //Chip default configuration
 	hi_u8 wbuf[5]={0x05,0x78,0xF7,0x90,0x02};   //Chip default configuration
    /*The CSN pin is masked when the byte is read and turned on when the EEP is written*/
    hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_GPIO);
    hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(HI_IO_NAME_GPIO_9, HI_GPIO_VALUE0);

	fm11_write_eep(0x3B1,1,&wbuf[1]);
	fm11_write_eep(0x3B5,1,&wbuf[3]);
}
/**
	@berf read EEPROM
	@param hi_u8 *dataBuff:Read data and save it in buff
	@param hi_u16 ReadAddr:Read address
	@param hi_u16 len:Read length
*/
hi_u32 fm11_read_eep(hi_u8 *dataBuff, hi_u16 ReadAddr, hi_u16 len)
{	
    hi_u32 status;

    status =write_read((hi_u8)((ReadAddr & 0xFF00)>>8), (hi_u8)(ReadAddr & 0x00FF), dataBuff, 2, len);

    return  HI_ERR_SUCCESS;
}
/**
	@berf Read NFC register
	@param hi_u16 addr:Read address
*/
hi_u8 fm11_read_reg(hi_u16 addr)
{
	hi_u8 pdata[10] ={0};
	hi_u8 a =0;

    if (fm11_read_eep(pdata, addr, 1) == HI_ERR_SUCCESS) {	
		a=pdata[0];
	    return a;
	} else {
      return HI_ERR_FAILURE;
    }   
}
/**
	@berf Write NFC register
	@param hi_u16 addr:write address
	@param hi_u8 data:write data
*/
hi_u8 fm11_write_reg(hi_u16 addr, hi_u8 data)
{
    hi_u32 status =0;

    status = write_fifo_reg((hi_u8)((addr & 0xFF00) >> 8), (hi_u8)(addr & 0x00FF), data);
    if (status != HI_ERR_SUCCESS) {
        return HI_ERR_FAILURE;
    }
    return HI_ERR_SUCCESS;
}
/*8
	@berf read FIFO
	@param hi_u8 NumByteToRead:Read data length
	@param hi_u8 *pbuf:Read the data and save it in buff
*/
hi_u8  fm11_read_fifo(hi_u8 NumByteToRead, hi_u8 *pbuf)
{
    hi_u8 read_fifo_len = NumByteToRead;

    if (fm11_read_eep(pbuf, FM327_FIFO, read_fifo_len)) {
        return HI_ERR_SUCCESS;
    } else {
        return HI_ERR_FAILURE;
    } 
}
/**
	@berf write FIFO
	@param hi_u8 *pbuf: Write the buff of FIFO
	@param hi_u8 len: Write FIFO length
*/
hi_u8 fm11_write_fifo(hi_u8 *pbuf, hi_u8 len)
{
    hi_u8 status =0;

    if (pbuf == NULL) {
        return HI_ERR_FAILURE;
    }
    status = write_fifo_data(pbuf, len);
    if (status != HI_ERR_SUCCESS) {
        return HI_ERR_FAILURE;
    }
    return HI_ERR_SUCCESS;
}

/**
	@berf Data postback
	@berf Send the label data in the ndef_file array to the mobile phone. After receiving the NFC label data, 
		  the mobile phone pulls up the corresponding app in the mobile phone
	@param hi_u32 ilen: Data length
	@param hi_u8 *ibuf: Data buff
*/
hi_void fm11_data_send(hi_u32 ilen,hi_u8 *ibuf)
{
	hi_u32 slen =0;
	hi_u8 *sbuf = NULL;

    if (ibuf == NULL) {
        return;
    }
	slen = ilen;
	sbuf = &ibuf[0];

	if (slen <= 32) {
		fm11_write_fifo(sbuf,slen);//write fifo	How many are there
		slen = 0;
        fm11_write_reg(RF_TXEN_REG,0x55);	//Trigger the non-contact interface to send back data when writing 0x55
	} else {
		fm11_write_fifo(sbuf,32);//write fifo    First send 32 bytes to FIFO
        fm11_write_reg(RF_TXEN_REG,0x55);	//Trigger the non-contact interface to send back data when writing 0x55

		slen = slen - 32;//Waiting length - 32
		sbuf = sbuf + 32;//Waiting data pointer + 32
        
        while (slen>0) {
            if ((fm11_read_reg(FIFO_WORDCNT_REG) & 0x3F )<=8) {
                if (slen<=24) {
                    fm11_write_fifo(sbuf,slen);//write fifo	First send 32 bytes to FIFO
                    slen = 0;	
                } else {
                    fm11_write_fifo(sbuf,24);			//write fifo	First send 32 bytes to FIFO
                    slen = slen - 24; 	//Waiting length - 24
                    sbuf = sbuf + 24; 	//Waiting data pointer + 24
                }
            }
        }
        irq_txdone = 0;
    }
}

/**
	@berf read RF data
	@param hi_u8 *rbuf: The read data is stored in rbuf
*/
hi_u32 fm11_data_recv(hi_u8 *rbuf)
{
	hi_u8 irq =0;
    hi_u8 ret =0;
	hi_u8 irq_data_wl = 0;
	hi_u8 irq_data_in = 0;
	hi_u32 rlen =0;
    hi_u32 temp =0;
#ifdef CHECK    
    /*Query method*/
	while (1) {
	    irq_data_wl = 0;
	    irq = fm11_read_reg(MAIN_IRQ);//Query interrupt flag
	    if (irq & MAIN_IRQ_FIFO) {
            ret=fm11_read_reg(FIFO_IRQ);
            if(ret & FIFO_IRQ_WL)  
            irq_data_wl = 1;
		}
        if (irq & MAIN_IRQ_AUX) {
            fm11_read_reg(AUX_IRQ);
            fm11_write_reg(FIFO_FLUSH,0xFF);
        }
		
	    if (irq& MAIN_IRQ_RX_START) {
          irq_data_in = 1;
		}

         if (irq_data_in && irq_data_wl) {
            irq_data_wl =0;
            fm11_read_fifo(24,&rbuf[rlen]);//Read 24 bytes after gradually full
            rlen += 24;
        }

        if (irq & MAIN_IRQ_RX_DONE) {
            temp =(hi_u32)( fm11_read_reg(FIFO_WORDCNT) & 0x3F);	//After receiving completely, check how many bytes FIFO has
            fm11_read_fifo(temp,&rbuf[rlen]);		//Read the last data
            rlen += temp;
            irq_data_in = 0;
            break;
        }
        hi_sleep(1);
	}
#endif 

#ifdef INTERRUPT
    while (1) {
	    irq_data_wl = 0;
	    irq = fm11_read_reg(MAIN_IRQ);//Query interrupt flag

	    if (irq & MAIN_IRQ_FIFO) {
            ret=fm11_read_reg(FIFO_IRQ);
            if(ret & FIFO_IRQ_WL)  
            irq_data_wl = 1;
		}
        if (irq & MAIN_IRQ_AUX) {
            fm11_read_reg(AUX_IRQ);
            fm11_write_reg(FIFO_FLUSH,0xFF);
        }
		
	    if (irq& MAIN_IRQ_RX_START) {

          irq_data_in = 1;
		}

         if (irq_data_in && irq_data_wl) {
            irq_data_wl =0;
            fm11_read_fifo(24,&rbuf[rlen]);//Read 24 bytes after gradually full
            rlen += 24;
        }

        if (irq & MAIN_IRQ_RX_DONE) {
            temp =(hi_u32)( fm11_read_reg(FIFO_WORDCNT) & 0x3F);	//After receiving completely, check how many bytes FIFO has
            fm11_read_fifo(temp,&rbuf[rlen]);		//Read the last data
            rlen += temp;
            irq_data_in = 0;
            break;
        }
		hi_sleep(1);
	}
#endif
	if (rlen <= 2) {
        return 0;
    }  
	rlen -= 2;//2bytes crc check
	return rlen;
}
/*Write FIFO and write register*/
hi_void fm11_t4t(hi_void)
{
    hi_u8 ret =0;
    hi_u8 nak_crc_err = 0x05;
    hi_u8 status_ok[3] = { 0x02, 0x90, 0x00 };
    hi_u8 status_word[3] = { 0x02, 0x6A, 0x82 };
    hi_u8 status_word2[3] = { 0x02, 0x6A, 0x00 };
    hi_u8 crc_err = 0;
    const hi_u8 ndef_capability_container[2] = { 0xE1, 0x03 };
    const hi_u8 ndef_id[2] = { 0xE1, 0x04 };
    hi_u8  i =0;
    hi_u8 xlen =0;
    hi_u8 xbuf[256] = {0};
    
    if (crc_err) {
        fm11_write_fifo(&nak_crc_err, 1);
	    fm11_write_reg(RF_TXEN_REG, 0x55);
	    crc_err = 0;
    } else {
        status_ok[0] = fm327_fifo[0];
        status_word[0] = fm327_fifo[0];
        status_word2[0] = fm327_fifo[0];
        // select apdu
        if (fm327_fifo[INS] == 0xA4) {
            if (fm327_fifo[P1] == 0x00) {
                if ((fm327_fifo[LC] == sizeof(ndef_capability_container)) && (0 == memcmp(ndef_capability_container, fm327_fifo + DATA, fm327_fifo[LC]))) {
                    fm11_write_fifo(status_ok, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = CC_FILE;
                } else if ((fm327_fifo[LC] == sizeof(ndef_id)) && (0 == memcmp(ndef_id, fm327_fifo + DATA, fm327_fifo[LC]))) {
                    fm11_write_fifo(status_ok, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = NDEF_FILE;
                } else {
                    fm11_write_fifo(status_word2, 3);
                    fm11_write_reg(RF_TXEN_REG, 0x55);
                    current_file = NONE;
                }
            } else if (fm327_fifo[P1] == 0x04) {
                ret = fm11_write_fifo(status_ok, 3);
                if (ret != HI_ERR_SUCCESS) {
                    printf("fm11_write_reg failed\r\n");
                }
                ret = fm11_write_reg(RF_TXEN_REG, 0x55);
                if (ret != HI_ERR_SUCCESS) {
                    printf("fm11_write_reg failed\r\n");
                }
            } else {
                fm11_write_fifo(status_ok, 3);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            }
        } else if (fm327_fifo[INS] == 0xB0) {
            if (current_file == CC_FILE) {
                fm11_write_fifo(status_ok, 1);
                fm11_write_fifo(capability_container + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo[LC]);
                fm11_write_fifo(&status_ok[1], 2);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            } else if (current_file == NDEF_FILE) {
                memcpy(&xbuf[0], &status_ok[0], 1);
                memcpy(&xbuf[1], &ndef_file[0] + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo[LC]);
                memcpy(&xbuf[0]+fm327_fifo[LC]+1, status_ok+1, 2);
                xlen=fm327_fifo[LC]+3;
                printf("xlen %d\r\n", xlen);
                fm11_data_send(xlen, xbuf);
            } else { 
                fm11_write_fifo(status_word, 3);
                fm11_write_reg(RF_TXEN_REG, 0x55);
            }
        } else if (fm327_fifo[INS] ==  0xD6) { // UPDATE_BINARY	
            for (i=0;i<rfLen;i++) {
                printf("0x%02x ",fm327_fifo[i]);	
            }
            printf("\r\n");
            memcpy(ndef_file + (fm327_fifo[P1] << 8) + fm327_fifo[P2], fm327_fifo + DATA, fm327_fifo[LC]);
            fm11_write_fifo(status_ok, 3);
            fm11_write_reg(RF_TXEN_REG, 0x55);
        } else {
            fm11_data_send(rfLen, fm327_fifo);
        }
    }
}

/* nfc display*/
hi_void *app_nfc_display(hi_void* param)
{
    for(;;) {
        oled_nfc_display(); 
        hi_sleep(1);
    }   
}
       
/*app nfc demo*/
hi_void *app_i2c_nfc_demo(hi_void* param)
{
    hi_u32 ret =0;
    hi_i2c_init(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE);//baud 400k
    hi_i2c_set_baudrate(HI_I2C_IDX_0, HI_I2C_IDX_BAUDRATE);
    /*Change NFC chip information, write EEPROM, usually do not open*/
    //nfc_init();
#ifdef  CHECK
    while (1) {
 
        rfLen = fm11_data_recv(fm327_fifo);		//Read RF data (one frame)			
        if(rfLen > 0){
            fm11_t4t();
            irq_data_in = 0;
        }
        hi_sleep(1);
    }
#endif 

#ifdef INTERRUPT
    while (1) {
        if (FlagFirstFrame) {	
			rfLen = fm11_data_recv(fm327_fifo);		//Read RF data (one frame)			
			if (rfLen > 0) {
			    fm11_t4t();
		    }
		irq_data_in = 0;
        // FlagFirstFrame =0;
		}
        hi_sleep(1);
	} 
#endif
}

/*c08i nfc task*/
hi_void app_c08i_nfc_i2c_demo_task(hi_void)
{
    hi_u32 ret =0;
    hi_task_attr attr = {0};
    hi_task_lock();
	/*attr Configuration of structural parameters*/
    attr.stack_size = C08I_NFC_DEMO_TASK_STAK_SIZE;//Task stack memory
    attr.task_prio = C08I_NFC_DEMO_TASK_PRIORITY;//The task priority ranges from 0 to 31. Tasks 0 to 10 should not be used. The SDK has been used. The higher the value, the lower the priority
    attr.task_name = (hi_char*)"app_i2c_nfc_demo";//Task name
	/*create task*/
    ret = hi_task_create(&g_c08i_nfc_demo_task_id, &attr, app_i2c_nfc_demo, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create i2c c08i_nfc demo task!\n");
    }
    hi_task_unlock();
}
/*nfc display task*/
hi_void app_nfc_display_task(hi_void)
{
    hi_u32 ret =0;
    hi_task_attr attr = {0};
    hi_task_lock();
	/*attr Configuration of structural parameters*/
    attr.stack_size = NFC_DISPLAY_TASK_STAK_SIZE;//Task stack memory
    attr.task_prio = C08I_NFC_DEMO_TASK_PRIORITY;//The task priority ranges from 0 to 31. Tasks 0 to 10 should not be used. The SDK has been used. The higher the value, the lower the priority
    attr.task_name = (hi_char*)"app_nfc_display";//Task name
	/*create task*/
    ret = hi_task_create(&g_nfc_display_task_id, &attr, app_nfc_display, HI_NULL);
    if (ret != HI_ERR_SUCCESS) {
        printf("Falied to create nfc display task!\n");
    }
    hi_task_unlock();
}