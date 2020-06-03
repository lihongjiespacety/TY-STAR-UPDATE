    MSS_CAN_init(&g_can0,
					CAN_SPEED_104M_500K,
                    (PCAN_CONFIG_REG)0,
                    32,
                    1);

    MSS_CAN_set_mode(&g_can0, CANOP_MODE_NORMAL);
    MSS_CAN_start(&g_can0); // Start the CAN

    /* Configure for receive */
    //ACR                            N_A: 0
    //                               RTR: 0
    //                               IDE: 1
    //                               ID(7:0)0x:00
    //   							 ID(15:8):0xF1
    //   							 ID(23:16):0x88
    //   							 ID(28:24):0x00

    //AMR                            N_A: 1
    //                               RTR: 1
    //                               IDE: 1
    //                               ID(7:0)0x:FF
    //   							 ID(15:8):0x00
    //   							 ID(23:16):0x00
    //   							 ID(28:24):0x1F
    //星敏nID:0CH
    //星务ID：00H
     pFilter.ACR.L=0x7+(CAN_Dev_ID<<13)*8 ;
     pFilter.AMR.L= 0x7+0x00001FFF*8;
     pFilter.AMCR_D.MASK= 0xFFFF;
     pFilter.AMCR_D.CODE= 0x0000;
    ret_status = MSS_CAN_config_buffer(&g_can0, &pFilter); //Message Buffer configuration
