proc debug_ {{msg ""}} {
    message $msg
    #debug break
}

ext debugdevice
#debug set_watchpoint write_io {0x2f} {} {debug_ "** $::wp_last_value received from debugdevice"}
