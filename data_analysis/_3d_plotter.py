# -*- coding: utf-8 -*-
"""
Created on Sun Oct 10 12:03:31 2021

@author: newbi
"""
from mpl_toolkits.mplot3d import Axes3D
from mpl_toolkits.mplot3d.art3d import Poly3DCollection
import matplotlib.pyplot as plt
import numpy as np

class RocketMesh:
    """ Representation of a rocket's geometry in space and methods to move it
    """
    
    def __init__(self, body_length, nosecone_length, cg_location_relative,
                 radius, discretization_theta):
        """
        
        :param body_length:
        :param nosecone_length:
        :param cg_location_relative:
        :param radius:
        :param discretization_theta:
        """
        self._body_length = body_length
        self._nosecone_length = nosecone_length
        self.cg_location = cg_location_relative
        self._radius = radius
        self._discretization_theta = discretization_theta
        
        self.position = np.array([0,0,0])
        
        self.bottom = None
        self.top = None
        self.tip = None
        
    @property
    def body_length(self):
        return self._body_length
    
    @property
    def nosecone_length(self):
        return self._nosecone_length
    
    @property
    def cg_location(self):
        return self._cg_location
    
    @cg_location.setter
    def cg_location(self, z_relative):
        self._cg_location = z_relative
    
    @property
    def radius(self):
        return self._radius
    
    @property
    def discretization_theta(self):
        return self._discretization_theta
        
    @property
    def position(self):
        return self._position
    
    @position.setter
    def position(self, coordinates):
        self._position = coordinates
        
    def set_rocket_rotation_euler(self, yaw, pitch, roll):
        pass
    
    def calc_bottom_vertices(self):
        center = self.position \
            - np.array([0,0, (1 - self.cg_location) * self.body_length])
            
        
        # generate the circle
        theta = np.linspace(0, 2 * np.pi, self.discretization_theta)
        circle = np.array([self.radius * np.cos(theta),
                           self.radius * np.sin(theta),
                           np.zeros(self.discretization_theta)])
        coordinates = center[:,None] + circle
        
        # TODO: add rotation here
        
        return [coordinates.T]
    
    def calc_side_vertices(self):
        center_bottom = self.position \
            - np.array([0,0, (1 - self.cg_location) * self.body_length])
        center_top = self.position \
            + np.array([0,0, self.cg_location * self.body_length])
            
        
        # generate the circles
        theta = np.linspace(0, 2 * np.pi, self.discretization_theta)
        circle = np.array([self.radius * np.cos(theta),
                           self.radius * np.sin(theta),
                           np.zeros(self.discretization_theta)])
        coordinates_bottom = center_bottom[:,None] + circle
        coordinates_top = center_top[:,None] + circle
        
        # generate the rectangles on the side
        rectangles = np.zeros((self.discretization_theta, 4, 3))
        rectangles[:,0,:] = coordinates_bottom.T
        rectangles[:,1,:] = np.roll(coordinates_bottom,-1,axis=1).T
        rectangles[:,2,:] = np.roll(coordinates_top,-1,axis=1).T
        rectangles[:,3,:] = coordinates_top.T
        
        # TODO: add rotation here
        
        return rectangles
    
    def calc_cone_vertices(self):
        center_top = self.position \
            + np.array([0,0, self.cg_location * self.body_length])
        tip = center_top + np.array([0,0, self.nosecone_length])
            
        
        # generate the circles
        theta = np.linspace(0, 2 * np.pi, self.discretization_theta)
        circle = np.array([self.radius * np.cos(theta),
                           self.radius * np.sin(theta),
                           np.zeros(self.discretization_theta)])
        coordinates_top = center_top[:,None] + circle
        
        # generate the triangles for the cone
        triangles = np.zeros((self.discretization_theta, 3, 3))
        triangles[:,0,:] = coordinates_top.T
        triangles[:,1,:] = np.roll(coordinates_top,-1,axis=1).T
        triangles[:,2,:] = tip
        
        # TODO: add rotation here
        
        return triangles


rocket = RocketMesh(1, 0.1, 0.5, 0.042, 20)

