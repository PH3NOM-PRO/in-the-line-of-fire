In The Line Of Fire - A Sega Dreamcast First Person Shooter written by Josh "PH3NOM" Pearson using KallistiOS.

This source code is being release under the GNU GPL license https://www.gnu.org/licenses/gpl-3.0-standalone.html

Project Notes:

This game is using a custom game engine and rendering pipeline written from the ground up using native PowerVR API's provided by KallistiOS.

This project was originally intended to develop an entire game engine, and In The Line Of Fire was the first game planned for the engine.  As the kickstarter campaign failed and my 3D artist and sound producers abondoned the project, and I stopped development at that time.  That said, this project is incomplete, and currently will only run the first level of In The Line Of Fire.  There is some cleanup and refatoring left to be desired, but this is the state of the project as-is.

Compilation Notes:

This demo is using more VRAM than is available by default using KallistiOS.  You will need to make two very minor changes per my thread here https://dcemulation.org/phpBB/viewtopic.php?t=104102 and then re-compile KOS.

This game includes FMV using a custom video decoder routing written by myself using LibMPEG for video http://libmpeg2.sourceforge.net/
and LibMPG123 for audio http://www.mpg123.org/api/
The source code to these projects are included here with a makefile for compiling for Dreamcast, as well as a pre-compiled lib. These libs will need to be included in your K:OS libarary directory to build the main makefile.


Model Format Notes:

The .md2 format used in this project is a custom variant of the traditional MD2 model format specification.  Primarily, the model vertices have been increased from 8-bit precision to 16-bit precision to eable much higher quality models.  I have lost the source to the exporter for this format which was a Blender plug-in forked off of the publically available eporter, so that is not included.  If you look closely, you will see I have done something kind of cool with the first person hand and weapon models, which is pre-computing back-face culling to eliminate roughtly 1/2 of the required vertex calculations with no penalty.

