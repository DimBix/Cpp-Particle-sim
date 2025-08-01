# Particle simulation
In this project I aim to simulate interactions between a large number of particles using opengl library to render the scene. As you can see by the commits history the main goal of the project has shifted from the original of simulating gravity, it was infact necessary to ensure the interactions between particles where correct and optimized before moving to the next step.

# MATH
I will now explain the math behind all the calculations in this simulation:
the law of motion behind every particle can be described by Verlet Integration $$
\mathbf{x}(t + \Delta t) = 2 \mathbf{x}(t) - \mathbf{x}(t - \Delta t) + \mathbf{a}(t) \Delta t^2
$$

# To Run the project you will need:
- g++ compiler
- library glfw3 installed (for windows it is already included)
- make

# Once ensured everything is met, run:
- make
- cd bin
- Execute the file

