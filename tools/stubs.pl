#!/usr/bin/perl

my $STUBS_NUM = 16;
my $STUB_FIRST_TASK = 0;

for (my $i = 0; $i<$STUBS_NUM; $i++){
    my $id = $i+$STUB_FIRST_TASK;
    my $pri = ($i+$STUB_FIRST_TASK)%8;
    #my $range = $STUB_FIRST_TASK+(7-$pri);

        print <<CODE2
void stub_task_${id}(void *arg)
{
    static uint32_t time_now = 0;
    //Priority is: ${pri}. Set in tasks.c
    while (1){
        time_now = timebase_show_ms();
        uint16_t delay = 1 + (lfsr_next()%3); //delay between 1-3ms
        rtos_delay(delay);
        time_now = timebase_show_ms();
        (void) time_now;
    }
}

CODE2
}



