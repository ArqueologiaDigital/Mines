proc test_joystick1 {} {
    set keyjoystick1.triga 1
    set keyjoystick1.trigb 2
    set keyjoystick1.up W
    set keyjoystick1.down S
    set keyjoystick1.left A
    set keyjoystick1.right D
    plug joyporta keyjoystick1
}

proc test_joystick2 {} {
    set keyjoystick2.triga 1
    set keyjoystick2.trigb 2
    set keyjoystick2.up W
    set keyjoystick2.down S
    set keyjoystick2.left A
    set keyjoystick2.right D
    plug joyport2 keyjoystick2
}

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

# Click on the middle button to confine mouse pointer inside OpenMSX window.
bind "mouse button2 down" toggle grabinput

