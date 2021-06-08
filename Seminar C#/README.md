# Introduction 

This application allows management, creation and editing of performances.

Application offers:

- creating bands and band members with images
- creating stages with images
- managing program of performances

# Environment

Windows 10 .NET 5.0

### Used packages

- MaterialDesignThemes
- Microsoft.EntityFrameworkCore
- Syncfusion.SfScheduler.WPF

# Authors

- Adam Kostolányi: xkosto04
- Samuel Valaštín: xvalas10
- Daniel Gavenda: xgaven08
- Martin Takács: xtakac07
- Filip Stupka: xstupk05

# Usage

Application is divided into multiple sections which are described below

### Program

- here is an overview of all performances in particular time slots shown in calendar
- one can click on particular performance to get details

### Performances

- here is an overview of all performances
- one can create new performance or edit one

### Band

- in this view one can create and edit a band with band members

### Stages

- here one can create and edit a stage

# Implementation details

### Collision detection of performances

Collision of two performances occurres, when on the same stage two performances have overlapping time slot.
If we assume that a time slot is an interval of ```timeStart``` and ```timeEnd``` and ```timeStart < timeEnd```,
then two intervals ```A = (timeStartA, timeEndA), B = (timeStartB, timeEndB)``` overlap when condition:
```timeStartA <= timeEndB and timeEndA >= timeStartB``` is true.

> What's the most efficient way to test two integer ranges for overlap?
> source: https://stackoverflow.com/questions/3269434/whats-the-most-efficient-way-to-test-two-integer-ranges-for-overlap

[1]: https://stackoverflow.com/questions/3269434/whats-the-most-efficient-way-to-test-two-integer-ranges-for-overlap