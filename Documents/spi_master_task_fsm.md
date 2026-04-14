### SPI master driver State Machine

```mermaid
stateDiagram-v2
    TASK_IDLE --> SPI_M_TASK_PREPARE_TRANS : Software event
    PREPARE_TRANS --> WAIT_RESOURCE
    WAIT_RESOURCE --> WAIT_NOT_BSY : Mutex available
    WAIT_NOT_BSY --> KICK_TRANS : Peripheral Not BSY
    KICK_TRANS --> WAIT_TRANS_DONE
    WAIT_TRANS_DONE --> PROCEES_DATA : Driver done signal
    
    PROCEES_DATA --> TASK_IDLE : No Errors
    PROCEES_DATA --> TASK_RECOVERY : Error
    TASK_RECOVERY --> TASK_IDLE
    
