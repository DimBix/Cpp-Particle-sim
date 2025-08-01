# Particle Simulation

In this project, I aim to simulate interactions between a large number of particles using the OpenGL library to render the scene. As you can see from the commit history, the main goal of the project has shifted from the original aim of simulating gravity. It was, in fact, necessary to ensure that the interactions between particles were correct and optimized before moving to the next step.

# Math

I will now explain the math behind all the calculations in this simulation. The law of motion for every particle can be described by Verlet Integration:

$
\mathbf{x}(t + \Delta t) = 2 \mathbf{x}(t) - \mathbf{x}(t - \Delta t) + \mathbf{a}(t) \Delta t^2 $

In this representation of the law of motion, velocity does not appear explicitly in the formula; therefore, we have to express it through:

$$
\frac{\mathbf{x}(t) - \mathbf{x}(t - \Delta t)}{\Delta t} $$

Thanks to these two formulas, it is possible to calculate the next position of the particle. With this achievement, we conclude the first part of the simulation. Next, we will handle collisions between walls and other particles. We assume that every particle has the same radius and mass as the others.

# To Run the Project, You Will Need:

- g++ compiler
- GLFW3 library installed (for Windows, it is already included)
- Make

# Once Everything is Set Up, Run:

1. `make`
2. `cd bin`
3. Execute the file
