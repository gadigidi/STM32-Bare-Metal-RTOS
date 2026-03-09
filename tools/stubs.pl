#!/usr/bin/perl

my $STUBS_NUM = 13;
for (my $i = 0; $i<$STUBS_NUM; $i++){
        print <<CODE
void stub_task_${i}(void *arg)
{
    static uint32_t counter = 0;
    static uint32_t time_now = 0;
    counter++;
    time_now = timebase_show_ms();
    uint16_t delay = lfsr_next() % 20;
    //uint32_t *sp = (uint32_t *)arg;
    do_nothing(time_now, counter);
    //stack_debug(sp + 9); //top of stack frame
    os_delay(delay);
    time_now = timebase_show_ms();
}

CODE
}



