import gym
import tensorflow as tf
from tensorflow import keras
import numpy as np


class Agent(keras.Model):

    def __init__(self, action_space):
        super(Agent, self).__init__()
        self.output_dim = action_space
        self.dense1 = keras.layers.Dense(32, activation='relu')
        self.dense2 = keras.layers.Dense(self.output_dim, activation='softmax')

    def call(self, inputs):
        x = self.dense1(inputs)
        return self.dense2(x)

    def act(self, observations):
        self.observations = np.expand_dims(observations, axis=0)
        self.raw = self.predict(self.observations)
        self.out = np.squeeze(self.raw, 0)
        self.action = np.random.choice(self.output_dim, 1, p=self.out)
        return self.action[0]


agent = Agent(2)
env = gym.make('CartPole-v0')
obs = env.reset()
agent.act(obs)
agent.load_weights('weights.h5')
for _ in range(1000):
    env.render()
    obs, _, _, _ = env.step(agent.act(obs))
env.close()
