#!/usr/bin/perl

my $STUBS_NUM = 13;
my $STUB_FIRST_TASK = 3;

for (my $i = 0; $i<$STUBS_NUM; $i++){
    my $id = $i+$STUB_FIRST_TASK;
    my $pri = ($i+$STUB_FIRST_TASK)%8;
    my $range = 3+(7-$pri);

        print <<CODE2
void stub_task_${id}(void *arg)
{
    static uint32_t time_now = 0;
    static uint8_t pri = ${pri};
    while (1){
        time_now = timebase_show_ms();
        //The higher the task priority, the less it releases the CPU
        //Demonstrates fairness vs. starvation of lower-priority tasks
        uint16_t delay = (lfsr_next()%${range})+1; //delay between 1-${range}ms
        os_update_counter(current_tcb->id);
        os_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

CODE2
}



