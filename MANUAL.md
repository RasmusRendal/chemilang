# chemilang manual
This manual is intended as an instruction in using the chemilang compiler.

### Prerequisites
This manual assumes that you have chemilang and crnsimul installed, and have them in your `$PATH`.
To compile chemilang, see the README in this repository.
To compile crnsimul, see the README in the [crnsimul repository](https://github.com/rasmusrendal/crnsimul).

## Table of contents
- [chemilang manual](#chemilang-manual)
		- [Prerequisites](#prerequisites)
	- [Table of contents](#table-of-contents)
		- [1. Hello world example](#1-hello-world-example)
		- [2. Syntax of Chemilang](#2-syntax-of-chemilang)
			- [2.1 Private species](#21-private-species)
			- [2.2 Output species](#22-output-species)
			- [2.3 Concentrations](#23-concentrations)
			- [2.4 Reactions](#24-reactions)
			- [2.5 Composition](#25-composition)
				- [2.5.1 Conditional Composition](#251-conditional-composition)
			- [2.6 Import Statement](#26-import-statement)
		- [3. Virtual environment](#3-virtual-environment)

### 1. Hello world example
We can't really do a canonical hello world in chemilang, due to the absence of strings.
We can, however, provide a simple example of a module that does multiplication.

Write the following text to a text file, ideally with the `.chem` file ending:
```
# multiply.chem
module main {
	private: [a, b];
	output: c;

	concentrations: {
		a := 5;
		b := 4;
	}

	reactions: {
		a + b -> a + b + c;
		c -> 0;
	}
}
```
When you have written and saved the file, you can execute the following command to compile it:
```command
$ chemilang multiply.chem
Output written to out.crn
```
This will produce a file named `out.crn`, which contains the compiled chemical reaction network. To simulate the file, simply invoke it as:
```command
$ ./out.crn
```
This will evaluate the CRN using the default shebang placed in the top-most line.
If you would like to use some other `crnsimul` options, you can edit the output file yourself, or simply call `crnsimul` directly with `crnsimul [options] out.crn`.

Additionally, the command line parameter `-O filename` is supported.
This is the only option supported in the chemilang compiler.
### 2. Syntax of Chemilang
That last example had a lot of code.
But what did it mean?

First of all, we defined a module named `main`.
A module is basically a chemical reaction network.
The strength of chemilang as a language is its ability to compose these modules. To create very complicated reaction networks, while still keeping the mental load managable.

The name is important, because this is the main "entry point" of the compiler into the reaction network.
If there is no module named main, the compiler will throw an error.

Inside the curly braces, a set of properties of the `main` module are defined.
The properties are:
 * private species
 * output species
 * concentrations
 * reactions

These four properties do not cover all properties available in chemilang, however.
Later, the properties composition and input will be covered.

First of all, we will explain the properties used in order.

Some general things to keep in mind about the syntax of chemilang is also that whitespaces are not meaningful, semicolons are used as a seperator, and that comments start with `#`, extending to the end of the line.

#### 2.1 Private species
Private species are species that are internal to a module.
There are no restrictions on private species, and any module can have them.

They are specified in the format `private: a;`, or if multiple are to be specified, `private: [a, b, c, ...];`.

If you define your private species twice, the second definition will be apppended to the first.

This also counts for input and output species.

#### 2.2 Output species
Output species are the species that contain the result of the computation that the module performs.

If we use composition, output species are visible to the supermodule.

But the output species in the main module are the species that will be plotted in the default shebang produced for crnsimul.

The format is the same as for private species, except that they are named output instead of private.

#### 2.3 Concentrations
Concentrations contain the initial concentrations of the species in a module.

Initial concentrations can only be set for private species.

They are of the format `a := 2`, where `a` can be replaced by any specie name, as long as it's defined in the private species, and `2` can be replaced by any integer.

#### 2.4 Reactions
Reactions are the bread and margarine of chemilang modules.

They are specified in the same format as in `crnsimul`.

Essentially, they are of the format `species -> species`, or `species ->(number) species`, if you want to define a reaction constant different from one.

The species can either be a single specie name, multiple specie names, or simply a `0`, to support the abstraction of an infinite fuel specie.

In contrast to `crnsimul`, it is illegal to define reactions that use species which have not already been declared either as input, output, or private species, and if you do this, the compiler will throw an error.

#### 2.5 Composition
The main point of chemilang, and the reason that it is useful to specify large chemical reaction networks in chemilang, instead of writing the reaction networks directly, is the composition function.

This feature allows the user to define submodules, which can be reused multiple times by different supermodules.

Take for instance, the submodule `Addition`:

```
# addition.chem
module addition {
	input: [x, y];
	output: z;
	
	reactions: {
		x -> x + z;
		y -> y + z;
		z -> 0;
	}
}
```
This module defines a module which performs addition.
Note that it does not have any start concentrations, but it does have input species.
The input species are the species which are connected to species from other modules.

If we wish to use this in a larger reaction network, save this network to a file named `addition.chem`, and open another file, `tripleaddition.chem`.
```
# tripleaddition.chem
import addition.chem;

module main {
	private: [a, b, c];
	private: i;
	output: d;

	concentrations: {
		a := 5;
		b := 7;
		c := 42;
	}

	compositions: {
		i = Addition(a, b);
		d = Addition(i, c);
	}
}
```
Note the import statement at the second line of the code.
This imports all the modules from the specified file.
To compile the example, simply put both files in the same directory, and type:
```command
$ chemilang tripleaddition.chem
Output written to out.crn
```
What the composition feature does is that it adds the reactions, private species, and initial concentrations of the submodule to the supermodule.
Additionally, the input and output, specified before the equals sign and inside the parentheses respectively, are aliased in the submodule.
That is, before the reactions are appended to the supermodule, any occurence of parameter #x is replaced by argument #x, akin to call-by-name in some programming languages, and Lambda calculus.

##### 2.5.1 Conditional Composition
Composition additionally supports adding an if statement inside the composition.
The format for this is as follows:

```
# tripleaddition.chem
import addition.chem;

module main {
	private: [a, b, c];
	private: i;
	output: d;

	concentrations: {
		a := 5;
		b := 7;
		c := 42;
	}

	compositions: {
		i = Addition(a, b);
		if (i) {
			d = Addition(i, c);
		}
	}
}
```
If statements are declared using the syntax `if (specie) { compositions }`, where `specie` is any specie in the current module, and `compositions`, is any set of compositions that can also be put in the root.

The if statements can be nested to produce something akin to and.

What the if statement does is that it adds the specie in the paretheses as a catalyst to all reactions in the compositions inside the culry braces. That is, if the compositions inside `compositions` defined the reactions:
```
a -> c;
b -> c;
```
and the specie inside the parentheses was `z`, the resulting reactions added to the supermodule would be:
```
a + z -> c + z;
b + z -> c + z;
```
This allows all sorts of interesting methods of fuzzy logic.
It can also be combined with the oscillator in the chemlib to produce sequential reaction networks, for instance.

### 2.6 Import Statement
Using a statement of the form `import file.chem` allows the user to import other files.
It will prefer files in the same directory.
However, it also searches in the chemilang library path, determined by the environment variable `$CHEMPATH`.
You can add a custom search path for Chemilang modules to your path by using the command `export CHEMPATH=/home/user/Projects/chemilang/chemlib`.
Multiple directiories can be added, like a standard UNIX search path.

If the file can be found in neither the current directory, or any of the directories defined by the environment variable, chemilang will also look in `/usr/local/share/chemlib` and `/usr/share/chemlib/` for any files.

### 3 Virtual environment
A virtual environment has been set up for Chemilang using VirtualBox.

Requirements:
- At least 2 available cores
- 2GiB of ram
- 12GiB of free disk space

These requirements are set to ensure a stable performance of the virtual machine, but can be changed in accordance with the users preferences.

The virtual machine runs Manjaro Linux with the Gnome desktop environment.
A user account is created with the username `chem` and the password `1234`.
The root password is also `1234`.

The source code for both Chemilang and crnsimul are included and placed in `~/Desktop`

In the folder `~/Desktop/examples` some small examples of Chemilang code can be found.

Chemilang and crnsimul have been installed in `/usr/bin` and can thus be invoked directly from the terminal.

A shebang is also added to the output files from Chemilang, which in turn means that these can be executed directly. An example of this is `./out.crn`, where `out.crn` is the output file generated by Chemilang.

The ChemVirtualbox can be downloaded [here](https://drive.google.com/open?id=12SyHMUCK8NPeMDToAoXVCJAOdb_GFwjo)
