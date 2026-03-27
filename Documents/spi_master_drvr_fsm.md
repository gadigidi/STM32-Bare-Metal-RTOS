### SPI master driver State Machine

```mermaid
stateDiagram-v2
    DRVR_IDLE --> WAIT_DUMMY_RXNE : Task Request
    WAIT_DUMMY_RXNE --> PRE_TXE : RXNE irq
    PRE_TXE --> WAIT_TXE
    WAIT_TXE --> WAIT_RXNE : TXE irq
    WAIT_RXNE --> READ_DR :RXNE irq
    READ_DR --> BYTE_DONE
    
    BYTE_DONE --> PRE_TXE : Transaction not done
    BYTE_DONE --> DRVR_FINISH: Transaction done
    DRVR_FINISH --> DRVR_IDLE
