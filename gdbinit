target remote :3333

mon reset halt
flushregs

tb heap_trace_start
commands
mon esp32 sysview start file:///tmp/heap.svdat
c
end

tb heap_trace_stop
commands
mon esp32 sysview stop
end

c
