if {![package vsatisfies [package provide Tcl] 9.0]} return
if {($::tcl_platform(platform) eq "unix") && ([info exists ::env(DISPLAY)]
	|| ([info exists ::argv] && ("-display" in $::argv)))} {
    package ifneeded tk 9.0.4 [list load [file normalize [file join $dir .. .. bin cygtcl9tk9.0.dll]]]
} else {
    package ifneeded tk 9.0.4 [list load [file normalize [file join $dir .. .. bin tcl9tk90.dll]]]
}
package ifneeded Tk 9.0.4 [list package require -exact tk 9.0.4]
