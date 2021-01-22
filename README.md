# Emotional AI

Hello, I am a student game development at the university of HOWEST in Belgium. 
As per our exam assignment, we were asked to make an implementation of something loosely related to AI.
For my assignment, I chose to make a basic implementation of emotion in AI.

From now on, I will be referring to my AI as my agent.
In this webpage, I'll explain how my implementation works.

## How to objectify Emotions

### The three factors
Of course, making an agent feel emotions just as humans do is impossible. 
What we can do is try to approach it as accurately as possible.

The way I tried to make my agent feel emotions is through 3 factors:
Mood, memory and personality. 

### Mood
Mood is the way a person feels in the moment, as time passes, mood is changed.
The different moods I implemented in my agent are anger, fear, and boredom.

Mood is affected by both memory and personality. 
Having a shorter memory-span can result in letting go of hatred or fear more easily. 
Personality affects the mood pretty logically, a timid agent will become scared more easily than a brave one.

### Memory
Memory is pretty self explanatory, simply put, it is how long you remember things happening to you. 

How is memory affected by mood and personality?

An agent who is bored (mood) will have it's memory decrease, since the longer it is bored, the more it will forget what happened before, since it wasn't interesting.
An agent with a inattentive personality will automatically have a shorter memory. This speaks for itself.

### Personality
Personality represents what emotions get triggered more easily by the agent.
There are of course personality traits wich are opposite of eachother.
A person who is timid, will have more trouble being brave, these two emotions could be considered opposites of each other. 

Personality is not affected by mood or memory, it is simply how the person or agent is. But personality plays a big role in how memory and mood are affected.


## Implementation

### Framework

Since this is a school project, I already had a framework that I could use. The framework I worked in is one where I had to program an agent to survive in a town with zombies, as you go into houses, pick up items and kill zombies, the difficulty increases.

### The emotions

Below is a screenshot of my code which relates to the emotions I gave my agent

![alt text](https://i.imgur.com/DCLpFrW.png)

### Interactions which affect mood and memory

Now to decide which interactions affect which emotions. This is where the programming of the emotions becomes related to how I would react to certain situations.

For example, I made it so that when the agent is excecuting his wandering behavior (where it just walks around, looking for houses to scanvenge items) for more than 5 seconds + it's inattentiveDedicated personality, he becomes more bored.

The more the agent becomes bored, the more he'll be tempted to start running in order to become less bored. 
The more he becomes bored, the more his memory decreases, because as humans work (or at least I do), when I become bored I want to forget about it and find something new.

![alt text](https://i.imgur.com/CChbGIp.png)

But if for example my agent get's bitten by a zombie, his memory will increase, and so will his anger.





