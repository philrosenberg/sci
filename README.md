Hi, Welcome to SciEng
-
SciEng is a collection of code to help you do science with C++. Don't think of it like one of the big numerics libraries - SciEng is not trying to be big or clever. It's trying to give scientists and engineers code that makes it easier for you to do nuts and bolts science. I come from a Physics background, I did my PhD in Planetary Science and Astronomy and I now work in atmospheric measurements (that's measuring the atmosphere, not doing measurements with excellent ambiance). I learned C++ when I was an undergrad and I kept using it. I needed a way to do all my basic day-to-day science things in the language that I am best at. This is things like applying calibrations, calculating parameters from other parameters, making plots (good enough to publish), correctly timestamping things and dealing with text that isn't just ASCII. All these things felt like they weren't as easy as they should have been in C++, so over my career I've been building a code base to make my life easier. SciEng is the best of that code base.

What Does SciEng provide?
-
SciEng gives you the following approximate functionality
- Compile time units compatibility - never get your units wrong again
- Any unit combinations
- Checking units are correct on assignment at compile time (never again multiply when you meant to divide a sensitivity)
- Easy to use UTC times
- Support for leap seconds
- Multi-dimensional arrays
- Elementwise calculations on multidimensional arrays
- Plotting
- Basic vector graphics

How Do I Install SciEng?
-
1) Clone or download the repository
2) Copy the include/sci directory to somewhere your compiler can find it
3) If you want to do plotting or graphics then install wxWidgets
4) You are done

SciEng is a header only library, so you don't have to build anything.

How Do I Use SciEng?
-
All you need to do is #include<> the header files you need to use. Because SciEng is a header ony library you don't need to link to anything. This also means you don't need to worry about matching compiler flags, compiler versions, etc.

Is SciEng "Modern" C++?
-
SciEng uses C++20 features. So remember to set your compiler to use this standard or later when utilising SciEng.

Is SciEng Stable and in Current Development?
-
As I write this (Jan 2026), SciEng has been on GitHub for a few weeks. I am updating it as fast as I can. I'm trying to get the API as stable as I can as fast as I can. I want to make the interface as generic as possible. There will be bugs, there may be API changes to deal with this. Consider this very much a beta release.

Can I use SciEng in Commercial Software, Are There Licence Restrictions On My Code If I Use SciEng?
-
I'm releasing this with an MIT licence. It would be nice if you credited it. But unlike GPL, you can use this code pretty much however you want - commercial software, or research software, and you don't need to release your software in any special way. See licence.txt for details.

How Do I Do Graphics and Plotting?
-
Graphics and on screen display is currently done via wxWidgets. This is a cross platform graphical user interface library. You need to download and install this separately. Check out wxWidgets.org for details. I am trying to set up writing graphics files without wxWidgets and add more generic ways to write to other graphics libraries.
