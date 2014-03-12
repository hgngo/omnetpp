#=================================================================
#  WINDOWS.TCL - part of
#
#                     OMNeT++/OMNEST
#            Discrete System Simulation in C++
#
#=================================================================

#----------------------------------------------------------------#
#  Copyright (C) 1992-2008 Andras Varga
#
#  This file is distributed WITHOUT ANY WARRANTY. See the file
#  `license' for details on this and other legal matters.
#----------------------------------------------------------------#


proc loadFile {win filename} {
    if [catch {open $filename r} f] {
       messagebox {Error} "Error: $f" info ok
       return
    }
    set contents [read $f]
    close $f

    set t $win.main.text
    set curpos [$t index insert]
    $win.main.text delete 1.0 end
    $win.main.text insert end $contents
    catch {$t mark set insert $curpos}
    $t see insert
}

proc saveFile {win {filename ""}} {
    global config

    if {$filename == ""} {
        set filename $config(log-save-filename)
        set filename [tk_getSaveFile -title {Save Log Window Contents} -parent $win \
                      -defaultextension "out" -initialfile $filename \
                      -filetypes {{{Log files} {*.out}} {{All files} {*}}}]
        if {$filename == ""} return
        set config(log-save-filename) $filename
    }

    if [catch {
       set f [open $filename w]
       set txt $win.main.text
       if {$txt == "..main.text"} {set txt .log.main.text}
       puts -nonewline $f [$txt get 1.0 end]
       close $f
    } err] {
       messagebox {Error} "Error: $err" info ok
    }
}

#
# Open file viewer window
#
proc createFileViewer {filename} {
    global config icons fonts help_tips B3

    if {$filename == ""} return

    # create a widget name from filename
    set w ".win[clock seconds]"
    if {[winfo exists $w]} {destroy $w}

    # creating widgets
    toplevel $w -class Toplevel
    wm focusmodel $w passive
    #wm maxsize $w 1009 738
    wm minsize $w 1 1
    wm overrideredirect $w 0
    wm resizable $w 1 1
    wm title $w $filename

    frame $w.toolbar

    packIconButton $w.toolbar.copy   -image $icons(copy) -command "editCopy $w.main.text"
    packIconButton $w.toolbar.find   -image $icons(find) -command "findDialog $w.main.text"
    packIconButton $w.toolbar.sep20  -separator
    packIconButton $w.toolbar.save   -image $icons(save) -command "saveFile $w $filename"
    packIconButton $w.toolbar.sep21  -separator

    set help_tips($w.toolbar.copy)   {Copy selected text to clipboard (Ctrl+C)}
    set help_tips($w.toolbar.find)   {Find string in window (Ctrl+F}
    set help_tips($w.toolbar.save)   {Save window contents to file}

    pack $w.toolbar  -anchor center -expand 0 -fill x -side top

    frame $w.main  -borderwidth 1 -relief sunken
    pack $w.main  -anchor center -expand 1 -fill both -ipadx 0 -ipady 0 -padx 0 -pady 0 -side top
    ttk::scrollbar $w.main.sb -command "$w.main.text yview"
    pack $w.main.sb -anchor center -expand 0 -fill y -ipadx 0 -ipady 0 -padx 0 -pady 0 -side right
    text $w.main.text -yscrollcommand "$w.main.sb set" -width 88 -height 30 -font $fonts(text)
    pack $w.main.text -anchor center -expand 1 -fill both -side left

    logTextWidget:configureTags $w.main.text
    bindCommandsToTextWidget $w.main.text

    # bind a context menu as well
    catch {$w.main.text config -wrap $config(editor-wrap)}
    bind $w.main.text <Button-$B3> [list textwidget:contextMenu %W "" %X %Y]

    # Read file
    loadFile $w $filename
}


#
# Create a context menu for a text widget
#
proc textwidget:contextMenu {txt w X Y} {
    global tmp config

    set tmp(wrap) [$txt cget -wrap]

    catch {destroy .popup}
    menu .popup -tearoff 0

    .popup add command -command editCopy -label {Copy} -accel {Ctrl+C} -underline 0
    .popup add separator
    .popup add command -command "editFind $txt" -label {Find...} -accel {Ctrl+F} -underline 0
    .popup add command -command "editFindNext $txt" -label {Find next} -accel {Ctrl+N,F3} -underline 5
    .popup add separator
    if {$w!=""} {
        .popup add command -command "LogInspector:openFilterDialog $w" -label {Filter window contents...} -accel {Ctrl+H} -underline 0
        .popup add separator
    }
    .popup add checkbutton -command "textwidget:toggleWrap $txt" -variable tmp(wrap) -onvalue "char" -offvalue "none" -label {Wrap lines} -underline 0
    .popup add separator
    .popup add command -command "$txt tag add sel 1.0 end" -label {Select all} -accel {Ctrl+A} -underline 0

    tk_popup .popup $X $Y
}

proc textwidget:toggleWrap {txt} {
    global tmp config

    $txt config -wrap $tmp(wrap)

    # set default for further windows
    set config(editor-wrap) $tmp(wrap)
}

proc textwidget:trimLines {t numlines} {
    if {$numlines==""} {return}
    set endline [$t index {end linestart}]
    if {$endline > $numlines + 100} {  ;# for performance, we want to delete in at least 100-line chunks
        set linestodelete [expr int($endline-$numlines)]
        $t delete 1.0 $linestodelete.0
    }
}


