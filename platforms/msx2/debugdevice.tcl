proc pause {{value 0}} {
    global use_pause
    if {$use_pause && $value == 0xff} { debug break }
}

proc pause_on {} {
    ext debugdevice
    set use_pause true
    debug set_watchpoint write_io {0x2e} {} {pause $::wp_last_value}
}

if { [info exists ::env(DEBUG)] } {
    set use_pause $::env(DEBUG)
    ext debugdevice
    debug set_watchpoint write_io {0x2e} {} {pause $::wp_last_value}
}

bind "mouse button2 down" toggle grabinput
plug joyporta mouse
