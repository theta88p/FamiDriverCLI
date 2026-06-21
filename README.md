# FamiDriverCLI
This is a sound driver that can be played on a Famicom/NES or emulator. This type writes MML and converts it with a compiler.
## Features
* You can use four types of envelopes: volume, pitch, note, and tone + software sweep.
* You can interrupt the sound and play another sound.
* There are loops, subroutines, macro functions, and a note map function to assign commands to scales.
## Syntax
* Define Track inside Music {}. The Music with the lowest number will be played first. 
* Track arguments are track number and device number. The device numbers 0 and 1 are square, 2 is a triangle, 3 is noise, and 4 is DPCM. 
* Track numbers can be assigned as you like from 0 to 15. For tracks with the same device number, the one with the lower number will be played first.
* The envelope is defined by @E number {}. The compiler converts the data into a set of numbers and the waiting frames, so writing the same numbers side by side will be wasteful. Please specify the waiting frame using the 'F' + value.
* Envelopes can be reused between different types. The first '|' is the location to loop back to, and the second '|' is the envelope after release. Both are optional. When '|' is 1, the release becomes invalid. If there is none, the return destination becomes the first. If you specify the envelope as @v0,10, the last number will be the delay specification (optional).
* The pitch envelope and detune values are added directly to the register values. The note envelope and software sweep start and end values are added to the note number in semitones (sweep increments are added to the register).
```
    #Title          "My Song"
    #Timebase   96
    #OffsetPCM  C000

    @E0 { 6 5 4 F4 | 3 | 0 }

    Music 0 {
        Track 0,0
            t150
            l16 o4 @v0 cdefgab>c

        Track 1,1
                .
                .
    }
```
* You can specify a note map with @M. This is for assigning commands to scales. Just write the note number to assign, the note number to convert, and the command, and the compiler will convert it.

* You can define DPCM with @dpcm. Please write the number and file path. This method uses the @ command to specify a file and select the playback frequency based on the scale. If you find it difficult to use, please use @M to assign it to a note.
* Even if you use key shift while using DPCM in note map,
It will not play properly because only the pitch changes, but the tone does not.
```

    @DPCM {
        0   kick.dmc
        1   snare.dmc
    }

    @M0 {
        c0  d+1 @0
        c+0 d+1 @1
    }
            .
            .
    Track 4,4
        l8 o0 @M0
        ccc+4ccc+4
```
## Commands
Please write commands with # at the beginning of the file.
command|description
:----|:----
#Title|||
#Artist|||
#Copyright|||
#Timebase|Number of frames per bar.|
#OffsetPCM|Command to offset DPCM data to make space.|

These are offset the numbers when calling the envelope.
* #OffsetV
* #OffsetF
* #OffsetF
* #OffsetT

Letters other than L, K, and Q are not case sensitive.
command|description
:----|:----
cdefgab|Note. If you add %, it is frame length. ~ is a grace note.
r|Rest.
r-|Disable envelope
<>|Relative octave
o|Absolute octave
[|Loop start
]|Loop end
:|End mid-loop
q|Gate time (Note length will be -n frames from end. Exclusive with others.)
u|Gate time (Note length will be n frames from start. Exclusive with others.)
Q|Gate time (Note length will be n/8 frames. Exclusive with others.)
k|Relative key shift
K|Absolute key shift
^|Tai
&|Slur
@|Tone
t|Tempo (BPM)
@p|Play the data of the specified song number.
@v|Specify volume envelope (Writing a comma and a number specifies a delay.)
@v*|Stop volume envelope
@f|Specify pitch envelope
@f*|Stop pitch envelope
@n|Specify note envelope
@n*|Stop note envelope
@t|Specify tone envelope
@t*|Stop tone envelope
L|Infinite loop
l|Default note length
v|Specify absolute track volume (0～15)
v+-|Specify relative track volume (-15～+15)
@d|Detune
hs|Hardware sweep (Arguments are rate, direction, amount)
he|Hardware envelope (Arguments are rate, loop or not)
s|Software sweep (Arguments are starting pitch, ending pitch, delay, speed; pitch difference: -64～63, speed: -7～-1 or 1～7)
s*|Stop software sweep
\ |Subroutine
$|Macro
pd|pseudo delay (Arguments are delay, volume, nth notes to shorten)
@m|Specify note map

For software sweep, a positive speed of 1～7 applies 3～9 right shifts to the current frequency value. A negative speed is the frame interval for adding one. Delay must be one of: 0～16, 18, 20, 22, 24, 28, 32, 36, 40, 48, 56, 64, 80, 96, 128, or 255 frames.
