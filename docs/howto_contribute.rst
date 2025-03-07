Prerequisite
============

For collaborative development, we use the "Fork & Pull" model from Github. So anyone who wants to contribute needs an account on Github. Then you need to fork the project you want to contribute.

.. note:: If you want to contribute with a new camera plug-in you should first request us (by email @ lima@esrf.fr) to get the new plug-in camera sub-module created. We will provide:

- a default structure of directories (``<mycamera>/src /include sip/ doc/ python/ test/``)
- the build system file (``<mycamera>/CMakeLists.txt``)
- templates files (src and include) for the mandatory classes:

- ``<MyCamera>Interface``
- ``<MyCamera>DetInfoCtrlObj``
- ``<MyCamera>SyncCtrlObj``

- a standard ``.gitignore`` file
- a template ``index.rst`` for the documentation

As above do not forget to fork the new sub-module project.

Create a github account
```````````````````````

This is an easy task, you just have to `Sign up <https://github.com/signup/free>`_, it's free!

Fork a project
``````````````

Check out the `Github doc <https://help.github.com/articles/fork-a-repo>`_, it is far better explained than we could do ;)


Contribute guideline
====================

It is very simple to contribute, you should follow the steps below.

1. Branch

First of all you have to create a branch for a new feature or for a bug fix, use an explicit
branch name, for instance "soleil_video_patch" .

2. Code/patch

If it's a patch from an existing module, respect and keep the coding style of the previous programmer (indentation,variable naming,end-line...).

If you're starting a new camera project, you've just to respect few rules:

- Class member must start with '**m\_**'
- Class method must be in **CamelCase**
- You must define the camera's namespace

3. Commit

Do as many commit as you need with clear comments.
Prefer an atomic commit with a single change rather than a huge commit with too many (unrelated) changes.

4. Pull Request

Then submit a `Pull Request <https://help.github.com/articles/using-pull-requests>`_

At this stage you have to wait, we need some time to accept or reject your request. So there are two possible issues:

1. The Pull-request is accepted, congrat!

We merge your branch with the the main project master branch, then everything is fine and you can now synchronize your forked project with the main project and go on with your next contribution.

2. The pull-request is rejected:

The pull request could be rejected if:

- the new code doesn't compile
- it breaks backward compatibility
- the python wrapping is missing or not updated
- the commit log message doesn't describe what you actually do

In case of a new camera plug-in sub-module the first pull request will be rejected if:

- as above
- the documentation is missing or if it does not fit with the guidelines (e.i :ref:`guidelines`)

We will tell you (code review on Github and/or email) about the reason and we will give some advises to improve your next tentative of pull-request.

So at this point you have to loop to item 2 (Code/Patch) again.
Good luck !

..  LocalWords:  namespace repo Github github
