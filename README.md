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

For example, I made it so that when the agent is excecuting his wandering behavior (where it just walks around, looking for houses to scanvenge items) for more than 5 seconds + it's inattentiveDedicated rating, he becomes more bored.

The more the agent becomes bored, the more he'll be tempted to start running in order to become less bored. 
The more he becomes bored, the more his memory decreases, because as humans work (or at least I do), when I become bored I want to forget about it and find something new.

![alt text](https://i.imgur.com/CChbGIp.png)

But if for example my agent get's bitten by a zombie, his memory will increase, and so will his anger.

Fear is also a mood in my agent, how quickly my agent becomes scared is in his timidBrave rating, there's only one thing that scares the player, which is being bitten by a zombie. Continue reading to read about how all these moods and personalities affect the agent's behavior.

The memory works as a timer, making the agent become more neutral over time. 
Whenever the memory timer is done counting, the players anger, boredom and fear wil decrease.

![alt text](https://i.imgur.com/ym84K5a.png)

### Mood points

I added mood points as a simple form of the mood factor, where if the mood points are higher than a personality trait, the mood that is affected is affected more heavily.
For example, say my agent has a calmHasty rating of 0.9 (meaning it gets angry very easily), and a zombie bites him.
This will make my agent more angry, but if my mood points are higher than it's calmHasty rating, his anger points (which represents how angry he is) will go up by a few extra points.

![alt text](https://i.imgur.com/7R2DThU.png)



## Effect on behavior

So now that we know how our mood and memory is affected by interactions and personality, how do these changes in mood and memory translate to his behavior?

We already talked about how if the agent becomes more bored, he'll be more likely to start running in order to become less bored in "Interactions which affect mood and memory", so I won't be going over that again.

### Fear

The more an agent becomes scared, the more his wandering behavior will be slower an shuddery. Humans have two reactions to fear, either to keep going and not think about it, or start becoming paranoid and turning around in fear. This is what I tried to portray in my timidBrave personality factor. 

For the purpose of showcasing the fear increasing as the agent gets hit, I made it so it doesn't go towards buildings, it only wanders around

![alt text](https://i.imgur.com/3tXkbuM.gif)

The more an agent becomes scared, the more it'll start shaking and quivering, of course, braver agents will be less affected by this, and will not be pushed into a state of fear as easily.

### Anger

The more an agent becomes angry, the more it'll feel the need to start turning around, looking for enemies to kill. Killing enemies lowers the agent's anger
If he gets too angry, however, he'll go into a blind rage, meaning that regardless if he has a gun, he'll start looking around. 

![alt text](https://i.imgur.com/0awnHt8.png)

As is logical, if the agent is rather calm than hasty, it will be easier for the agent to remain calm. 

## Testing

While testing my agent, it became apparent that the behaviors sometimes yielded an agent who could survive in this zombie survival game for longer, the getting angry would sometimes help my agent shoot zombies that were running behind him, but were not in his field of view, making it so he shot zombies before they had a chance to approach.

The fear emotion was often less helpful, since it slows the character down to look around more, meaning it would be harder to find new houses with items in them, on the other side, this resulted in the character becoming bored more quickly, since it's not discovering anything new.
This resulted in the agent wanting to run, making it faster.

Everytime the emotion timer went off (decreasing the anger, boredom and fear factors), I would print the agent's emotional state to the console. 
So I could get feedback on how scared, angry or bored he was at that time.

For the purpose of showcasing, I made my memory very low, so the agent quickly forgets about what happened a few seconds before.

![alt text](https://i.imgur.com/6gN9xGr.gif)



## Result

In the end I was able to create an AI with a simplistic understanding of some basic human emotions. 
Through my own observations I found that some combinations of personality traits worked better than others, such as a brave and hasty character. Or an inattentive and timid one.

## Conclusion
Overall I had a lot of fun implementing this into my zombie survival game, and I could definetely see the benefits of these mechanics in video games. 
Story driven video games where the player has a lot of interaction with the AI such as Mass Effect could benefit from this, opening the player to different playstyles, where characters have an (artificial) emotional connection to the player, opening up a plethora of interesting game mechanics.

Some things were unfortunately unable to be implemented due to the restrictions of the framework, I would have like to make hasty agents hold grudges against zombies who previously hit them. 

## Future Work

As I said, some things were unimplemeted due to framework restrictions, but some things were also unable to be implemented because they wouldn't serve any significant purpose.
Having the emotion Joy, making the player happy upon certain interactions would definetely be fun, but as for the game I worked with, I didn't see how the character feeling joyous could have any difference on how it behaves.


