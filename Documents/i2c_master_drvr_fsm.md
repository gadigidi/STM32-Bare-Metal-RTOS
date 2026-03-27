### I2C Write driver State Machine

```mermaid
stateDiagram-v2
    IDLE --> SEND_ADDR_W : SB
    SEND_ADDR_W --> WAIT_ADDR_W
    WAIT_ADDR_W --> WAIT_TXE : ADDR
    WAIT_TXE --> SEND_BYTE : TXE
    SEND_BYTE --> WAIT_TXE : More bytes
    SEND_BYTE --> WAIT_BTF : Last byte
    WAIT_BTF --> GEN_STOP : BTF
    GEN_STOP --> IDLE

    WAIT_ADDR_W --> ERROR : I2C_ER
    WAIT_TXE --> ERROR : I2C_ER
    SEND_BYTE --> ERROR : I2C_ER
    WAIT_BTF --> ERROR : I2C_ER
    
    note right of ERROR
      Will be fixed by task
    end note
