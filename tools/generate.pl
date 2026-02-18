#!/usr/bin/perl

my @m_d_states = qw(
    M_DRVR_IDLE
    M_DRVR_GEN_START
    M_DRVR_WAIT_SB
    M_DRVR_SEND_ADDR_W
    M_DRVR_WAIT_ADDR_W
    M_DRVR_TX_BYTE
    M_DRVR_TX_WAIT_BTF
    M_DRVR_TX_DECIDE_STOP_OR_RESTART
    M_DRVR_GEN_RESTART
    M_DRVR_SEND_ADDR_R
    M_DRVR_WAIT_ADDR_R
    M_DRVR_RX_SETUP_N
    M_DRVR_RX_STREAM
    M_DRVR_RX_LAST_2
    M_DRVR_RX_LAST_1
    M_DRVR_GEN_STOP
    M_DRVR_DONE
    M_DRVR_ERROR
    );

my @m_t_states = qw(
    M_TASK_IDLE
    M_TASK_PREPARE_TRANS
    M_TASK_INITIATE_TRANS
    M_TASK_WAIT_TRANS_DONE
    M_TASK_PROCEES_DATA
    M_TASK_RECOVERY
);


foreach my $state (@m_d_states){
    printf ("    case %s: {\n", $state);
    printf ("        if (){\n");
    printf ("            next_state = ;\n");
    printf ("        }\n");
    printf ("        break;\n");
    printf ("    }\n\n");
}


foreach my $state (@m_t_states){
    printf ("        case %s: {\n", $state);
    printf ("            if (){\n");
    printf ("                next_state = ;\n");
    printf ("            }\n");
    printf ("            break;\n");
    printf ("        }\n\n");
}

