### I2C Write driver State Machine

```mermaid
stateDiagram-v2
    [*] --> M_DRVR_IDLE

    M_DRVR_IDLE --> M_DRVR_SEND_ADDR_W : SB
    M_DRVR_SEND_ADDR_W --> M_DRVR_WAIT_ADDR_W
    M_DRVR_WAIT_ADDR_W --> M_DRVR_WAIT_TXE : ADDR
    M_DRVR_WAIT_TXE --> M_DRVR_SEND_BYTE : TXE
    M_DRVR_SEND_BYTE --> M_DRVR_WAIT_TXE : More bytes
    M_DRVR_SEND_BYTE --> M_DRVR_WAIT_BTF : Last byte
    M_DRVR_WAIT_BTF --> M_DRVR_GEN_STOP : BTF
    M_DRVR_GEN_STOP --> M_DRVR_IDLE

    M_DRVR_WAIT_ADDR_W --> M_DRVR_ERROR : I2C_ER
    M_DRVR_WAIT_TXE --> M_DRVR_ERROR : I2C_ER
    M_DRVR_SEND_BYTE --> M_DRVR_ERROR : I2C_ER
    M_DRVR_WAIT_BTF --> M_DRVR_ERROR : I2C_ER
    
    note right of M_DRVR_ERROR
      Will be fixed by task
    end note
