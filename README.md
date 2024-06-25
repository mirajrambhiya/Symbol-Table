# Scope-based Variable Management System

## Overview
This project implements a scope-based variable management system in C. It uses a hash table to store variables and their values within different scopes. The system supports creating and finalizing scopes, assigning values to variables, and printing variable values. The system reads commands from an input file to perform these operations.

## Features
1. **Initialize Scope**: Create a new scope and link it to the current scope.
2. **Finalize Scope**: Finalize the current scope and return to the previous scope.
3. **Assign Variable**: Assign a value to a variable in the current scope.
4. **Print Variable**: Print the value of a variable, looking up through the scopes.
