# RaspiGFX


![alt text](https://github.com/wiredolphin/wiredolphin.github.io/blob/master/wiki/img/RaspiGFX_logo_2.jpg "RaspiGFX")

<br>
<h2>A graphic library for Raspberry Pi that allows to drive small TFT display modules through SPI.</h2>

<h3>Prerequisite</h3>
<ul>
	<li>Enable SPI on the Raspberry is required. <a href="https://github.com/wiredolphin/RaspiGFX/wiki/Enable-SPI-on-Raspberry-Pi">Here</a> 
		a follow up on one method to do it.
	</li>
	<li>You must load the <b>fbtft</b> kernel module before to use this library. This module allows
		to use the Linux framebuffer and draw to a connected display through the <i>Serial Peripheral Interface</i>. 
		For default the Raspberry framebuffer is set to 16 bits-per-pixel.
		For more info see <a href="https://github.com/notro/fbtft">this</a> <em>GitHub</em> repo and
		the wiki you can find here <a href="https://github.com/notro/fbtft/wiki">https://github.com/notro/fbtft/wiki</a>.
	</li>
</ul>
<br \>
<h3>How to use it</h3>
<h5> > Download/clone and compile example</h5>
<p>Download/clone this repo, <code>cd</code> into the <code>RaspiGFX</code> folder and compile:</p>
<pre>
cd RaspiGFX
g++ -std=c++11 RaspiGFX.cpp RGB565.cpp Framebuffer.cpp <b>your_cpp_source.cpp</b> -o rgfx
</pre>
or
<pre>
g++ -std=c++11 RaspiGFX.cpp RGB565.cpp Framebuffer.cpp graphicTest.cpp -o rgfx
</pre>
<div>if you want to use the <code>graphicTest.cpp</code> program.</div>
<br \>
<p>Anyway it is suggested to examine the example program source to get an idea on how to use the <code>RaspiGFX</code> class.</b>
<br \>
<h5> > Load the <b>fbtft</b> kernel module</h5>
<p>
	Since version 3.15 the Linux Kernel comes with included <b>FBTFT</b> drivers. You need to load it the first time
	after boot before to use this library. This is accomplished issuing the fallowing command:
</p>
<pre>sudo modprobe fbtft_device name=adafruit22a rotate=90</pre>
<div>
	where:
	<ul>
		<li><code>name</code> is the device name required for the model of display you are using. 
		(<i>ili9340</i> in this case, <a href="https://github.com/notro/fbtft/wiki/LCD-Modules">here</a> is the full list of supported displays and the corresponding name to be used).
		</li>
		<li><code>rotate</code> is the screen orientation.</li>
	</ul>
</div>
<div>You can also make it load automatically when the Rpi boots.</div>
<p>If required you can unload the module issuing:</p>
<pre>sudo modprobe -r fbtft_device</pre>
<br \>
<h5> > Use the included test program</h5>
<div>Execute with the fallowing command:</div>
<pre>sudo ./rfgx</pre>
<div><i>Note that the test program comes with the <b>fbtft</b> module parameters suitable for a <b>ili9340</b> display. You need to change 
the device name according to the display you are using.</i>
</div>
<br \>
<h3>Further info</h3>
<div>
<b>RaspiGFX</b> comes with a built-in font and some custom fonts you can find within the <code>BitmapFonts</code> folder. You
can build your own custom font starting from <code>.ttf</code> files converting them with the <code>fontconvert</code> easy-to-use tool provided by <b>Adafruit</b>. For more info and a guide on how to use such tool you can follow the learn center on the Adafruit site <a href="https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts">here</a>.
</div>
<br \>
<h3>License notes</h3>
<div>This C++ library is written by the owner of this repo, <a href="https://github.com/wiredolphin">wiredolphin</a>. You can freely use, modify or redistribuite it without any warranty. It only requires
to include credit or mention of the contributors. Read the license header in each source file for more
info.</div>
<br>
<div>Note that this code contains some algorithms taken from the <b>Adafruit_GFX</b> library for drawing some primitives and for drawing and use the custom bitmap fonts, so you must also agree their license terms. For more info look at the <a href="https://github.com/adafruit/Adafruit-GFX-Library">Adafruit Industries</a> <i>GitHub</i> repo.
</div>







