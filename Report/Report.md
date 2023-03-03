# Phased Array Ultrasonic Transmitter
# Abstract
I built a phased array ultrasonic transmitter. The original plan was also to make a receiver to make a 3D scan, but due to time constraints the receiver was out-of scoped. See [Appendix A](#appendix-a-receiver-notes) for notes on how the receiver should work. The goal was to be able to significantly direct an ultrasonic transmission in two dimensions.

# Table of Contents

# Phased Arrays Backround
Phased arrays are a way to direct to direct the output of individually controlled antennas to point in a certain direction.

![Figure 1: 2D Phased Array Diagram](https://upload.wikimedia.org/wikipedia/commons/4/4a/Phased_array_animation_with_arrow_10frames_371x400px_100ms.gif)
#### Figure 1 Phased Array Diagram

To control the direction of the output one can carefully adjust the relative phases of the transmitters such that the peak of all of the emitted waves align in a certain direction. Essentially this creates constructive interference pattern in the desired direction and varying degrees of destructive interference in all other directions (See [Appendix B](#appendix-b-interference-pattern-of-phased-array) for interference pattern). The simplest case of this is simply having each emitter having the same phase, creating a wave going perpendicular to the array of emitters.

## One Dimensional Phased Array
![One Dimensioplanenal Phased Array Diagram](https://www.radartutorial.eu/06.antennas/pic/ps.big.jpg)

To find the phase delay of each emitter, define emitter the at position n in the line as $p_n$. To create a one dimensional phased array at some angle $\Theta_s$, draw a line perpendicular line to the desired angle through each emitter. Define $L_n$ as the line passing through $p_n$. To calculate the phase delay from $p_m$ to $p_n$, calculate the shortest distance from $p_m$ to $L_n$, at angle $\Theta_s$. Call this distance $x_{m, n}$. If the distance between emitters, m and n is $d_{m,n}$, then:
$$ x_{m,n} = d_{m,n}sin(\Theta_s) $$
To constructively interphase, the peak of the wave must reach the intersection point on $L_n$ exactly when $e_m$ starts emitting the wave. Therefore the phase delay is:
$$\Delta\theta_{m,n} = \frac{2\pi x_{m, n}}{\lambda}$$
Where $\lambda$ is the wavelength of the emitted wave.
Therefore 
$$\Delta\theta_{m,n} = \frac{2\pi d_{m,n}sin(\Theta_s)}{\lambda}$$

## Two Dimensional Phased Arrays
For a two dimensional array, the idea is the same, except instead of creating a line $L_n$, create a plane that is normal to the desired emission direction. Then calculate the distance from each emitter to that plane.

![3D Plane Visualization](figures/2DPhasedArrayMathVisualization.png)

If the desired direction is defined in terms of  ($\theta$, $\phi$). Then a normal vector to that plane is:
$$ N = <x_n, \ y_n, \ z_n> = <sin(\theta)cos(\phi), \ sin(\theta)sin(\phi), \ cos(\theta)>$$

In reality, the plane must pass through the emitted that must emit last of all the emitters in the array. This will always be one of the corners of the array. If the corner emitted has coordinates $c=(x_c, y_c, 0)$, then the plane can be defined as

$$ x_nx+ y_ny + z_nz + D = 0$$
Where:
$$ D = -(x_nx_c+y_ny_c) $$

The distance from any point $(x_p, y_p, z_p)$ to this plane is thus:
$$ d_p = |x_nx_p+y_ny_p+z_nz_p+D|$$

It is also useful to define the farthest away corner point $f=(x_f, y_f, z_f)$. Since this emitted needs to be activated first is gets a phase shift of zero. Its distance to the plane is defined as $d_f$. Thus, the time phase shift for an emitter $e$ is:
$$t_{phase \ shift} = d_f - d_e$$
Therefore the angular phase shift $\alpha$ is 
$$\alpha = \frac{2\pi t_{phase \ shift}}{\lambda} $$


I have made a [visualization of this setup available here](https://www.geogebra.org/m/fzej5fvj).

# Electronics Design

## Block Diagram
I followed the basic hardware design of another [ultrasonic array](https://doi.org/10.3390/app11072981), this one designed for levitation, but modified the design to suit my components better. Phased arrays often require more IO than is on a controller, so they devised a system to get around this they use a bunch of row addressable shift registers. 
![Block Diagram](figures/UltrasonicPhasedArrayBlockDiagram.png)
Note that only one row is show, but in the actual design there are 6 rows, each with its own $R/\bar{W}$ Signal.

## Hardware Description
Depending on the $R/\bar{W}$ value, the shift registers can rotationally shift their inputs, or be written to from the microcontroller. This way, it is possible to write to each row individually by only keeping one rows $R/\bar{W}$ high at a time. If no $R/\bar{W}$ signals are high, all of the shift register rotationally shift. This is the operation mode when generating a series of pulses. The shift registers are clocked using a multiple of the desired clock frequency for my ultrasonic emitters (40kHZ). For example if using 64-bit shift registers, the shift registers should be clocked at $40kHz*64=2.56MhZ$.

Each shift register has a shifted (based on the desired phase shift_ version of 
```
1111111111111111111111111111111100000000000000000000000000000000
```
stored inside it.
### Table of Examples

| Phase Shift      | Shift Register Contents                                            |
| ---------------- | ------------------------------------------------------------------ |
| $\frac{\pi}{32}$ | `0111111111111111111111111111111110000000000000000000000000000000` |
| $\pi$            | `0000000000000000000000000000000011111111111111111111111111111111` |
| $\frac{3\pi}{2}$ | `1111111111111111000000000000000000000000000000001111111111111111` |

## Part Choices

I settled on using [TR40-16/T](https://www.estudioelectronica.com/wp-content/uploads/2018/09/SHT-USW-1.pdf) ultrasonic transmitters based mainly on there low price and relatively good availability. Though to get them cheaply I bough them from an [AliExpress seller](https://www.aliexpress.us/item/2255800237971099.html). Based on the paper I used [TC4427](https://ww1.microchip.com/downloads/en/DeviceDoc/20001422G.pdf) MOSFET drivers. To allow me to get up to $\frac{\pi}{32}$ phase resolution, I wanted to use 64 bit shift registers. I also wanted to be able to prototype on a breadboard, so I used the the [CD4517B](https://www.ti.com/lit/ds/symlink/cd4517b.pdf), The only generally available through-hole 64 bit shift register. I chose the [SNHC175](https://www.ti.com/product/SN54HC157) multiplexer due to its low price, and wide availability in a variety of form factors.

### RP2040
I choose to use the RP2040 microcontroller for this project. This is primarily due to its low cost (~$1), and relatively high speed. The RP2040 also has PIO blocks that allow for clock cycle precise timing, which is essential to my hardware design.

### RP2040 PIOs
The RP2040 has 2 programmable IO blocks (PIOs) each of which has four state machines (SMs). These are essentially tiny co-processors that are terrible for general purpose computing, but designed for time-sensitive IO operations. These state machines run a limited [ISA consisting of 9 instructions](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf#instructionSet). Importantly, each instruction takes exactly one clock cycle, and the clock frequency can be set using a divider of the 150MHz system clock. 

To get an ultrasonic response, I need to send out a series of pulses and then stop. While it is relatively easy to send out a continuous series of pulses using clock dividers or the PWM peripheral, the best wat I found to send out a set series of precisely timed pulses was using a PIO SM. To achieve the correct timing, the PIO frequency is set to:

$$f_{pio} = f_{ultrasonic}*n_{steps}*n_{instructions \ per \ step}$$

Here, $n_{steps}$ is the number of bits of the shift register (64), and $n_{instructions \ per \ step}$ is 4 due to [my pio code](https://github.com/abezukor/PhasedArrayUltrasonic/blob/main/3DArray/Software/pulses_pio/pulses.pio#L14). ( The `[1]` indicates one no-op after the instruction). Therefore my PIO clock frequency is:

$$f_{pio} = 40kHz*64*4=10.24MHz$$


To create a set number of pulses, as opposed to a stream of them, my PIO code waits for an input from a hardware FIFO, then loops that many times generating a pulse each time. Once its done, it generates an interrupt to let the main code know the pulses have been sent. Afterwards, it waits for another input on the hardware FIFO. 

# First Prototype
![First Prototype](figures/FirstPrototype.jpg)
As a first prototype, I built one row of my circuit on a breadboard. This allowed me to diagnose the circuit and make changes easily. An oscilloscope trace of pulse bounced back is below. I was successfully able to get a noticeable difference when directing the pulse at an angle verses straight ahead. 

![1 Row Return Ocilliscope Trace](figures/OneRowOcilliscopeTrace.jpg)
Here the bottom signal is the pulses to the emitters, while the top signal is the microphone response.

## 1D Array Demonstration Video
In the following demonstration, the phased array is set to $15^\circ$ from the centerline.
[![1D Phased Array Row Test](https://res.cloudinary.com/marcomontalbano/image/upload/v1677865515/video_to_markdown/images/youtube--Lo8rY6ppoV0-c05b58ac6eb4c4700831b2b3070cd403.jpg)](https://youtu.be/Lo8rY6ppoV0 "1D Phased Array Row Test")


# Final Design
After my first prototype, I designed a PCB for one row, with the idea that 6 of them could be combined together to make a 6x6 array. To differentiate each row, a series of 0603 resistor pads are used as jumpers, with only one being connected on each board. These control the $R/\bar{W}$ signal for each PCB. I changed to SMD parts where applicable and ordered them from JLCPCB. KiCAD files are included in the provided files. Unfortunately I made a mistake in my initial PCB order, so the inputs to the column 2 and column 6 multiplexers were swapped. The the PCBs have to be modified to account for this. The included schematic and layout have been updated to fix this issue.
![Front of the PCB](figures/PCBFront.jpg)
![Back of the PCB](figures/PCBBack.jpg)

## PCB Verification
To verify that the PCB functions as intended I wrote code that sets each emitter to a phase delay of 2 from the last emitted. Then measured the output of each by using the digital probes on an oscilloscope. Once the issues were solved the traces looked like the following (Showing columns 2, 3, 4 on rows 1, 3, 4, 5):

![Oscilloscope showing phase shift](figures/PCBPhaseShiftWorking.png)

The exact phase delay was also measured to verify that everything was functioning as expected.

# Software
The software is written in C++ using the [RP2040 SDK](https://github.com/raspberrypi/pico-sdk). It is split into two threads (each running on one of the RP2040's two cores). The main thread listens for Serial input to change the the angles ($\theta/\phi$), and when it receives new data it stores the new data and sets an `updated` flag. 

The second thread actually controls the ultrasonic emitters. When it receives a new set of angles, it [calculates the phase shifts](#two-dimensional-phased-arrays), then loops, putting out a series of 24 pulses using the PIO, and waiting 20ms for a response. The wait time is needed so the receiver does not receive data directly from the transmitters, but the ultrasonic bounce back. 20ms was chosen as the wait time as it is a good balance between frequent pulses, and being able to detect far away objects. Due to the speed of sound, a 20ms wait allows detection of objects up to 3.43m away. Waiting longer does not provide a significant benefit, as the return strength at that distance is barely detectable.

[The full code can be found on here](https://github.com/abezukor/PhasedArrayUltrasonic/tree/main/3DArray/Software).

# Outcome
Due to limited time and breaking two bones in my arm, I was not able to fully diagnose the software. As far as I can tell, there is no significant beamforming using the 2D array, evan when only beamforming in one dimension. I was able to verify the functionality of the hardware, so I am confident that this can be fixed in software.
# Code and KiCAD
All the code and KiCAD files are [on this github](https://github.com/abezukor/PhasedArrayUltrasonic).
# References
- Chetvorno, CC0, via Wikimedia Commons https://commons.wikimedia.org/wiki/File:Phased_array_animation_with_arrow_10frames_371x400px_100ms.gif
- https://www.radartutorial.eu/06.antennas/Phased%20Array%20Antenna.en.html
# Appendix A: Receiver Notes
After some testing, the received voltage is about ~75mV. It also greatly improves accuracy to pass the received signal through a band-pass filer to only allow 40kHZ signal in. I had initially planned to do this using a Chebyshev filter using op-amps. This would have been connected to one of the analog inputs of the RP2040.

On the software side, after a pulse is sent out, I would start recording the analog samples using DMA ([similar to this example](https://github.com/raspberrypi/pico-examples/tree/master/adc/dma_capture)), the sampling frequency must be above the Nyquist frequency of 80KHz, but it may be beneficial to reduce it from the RP2040s maximum sampling frequency of 500kHz. After the wait period has passed, the RP2040 could process the data and find the peak of the received wave to determine the distance to an object in the current direction.

# Appendix B: Interference Pattern of Phased Array  
![Phased Array Interpherence Pattern](https://upload.wikimedia.org/wikipedia/commons/2/28/Phased_array_radiation_pattern.gif)