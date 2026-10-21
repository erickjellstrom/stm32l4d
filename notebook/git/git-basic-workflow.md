# github workflow:

1. Clone the Repository

Replace the URL with your actual GitHub repository URL.
bash
git clone https://github.com
Note: After cloning, remember to navigate into the new folder using cd repository-name before running the next commands.

2. Create and Switch to a New Branch

Replace my-feature-branch with a short, descriptive name for your task.
bash
git checkout -b my-feature-branch
(Modern alternative: git switch -c my-feature-branch)

3. Stage Your Changes 

bash
git add .

4. Commit Your Changes

bash
git commit -m "Add login authentication logic"

5. Push to GitHub (For a Brand New Branch)

Because this is the first time you are pushing this specific branch to GitHub, you need to set the "upstream" destination.
Use this exact command:
bash
git push --set-upstream origin my-feature-branch
(Shortcut: You can also use git push -u origin my-feature-branch. For any future pushes on this same branch, you can just type git push)

6. Pull Request and Merging

These steps are typically done directly on the GitHub website UI rather than the terminal:
1. Go to your repository on GitHub.com.
2. Click the green "Compare & pull request" button that automatically appears.
3. Review, click "Create pull request", and once approved, click "Merge pull request".
4. Click "Delete branch" on the webpage to keep it clean.

7. Clean up

Once the code is merged on GitHub, bring your local main branch up to date and delete your local copy of the feature branch:
bash
git checkout main
git pull
git branch -d my-feature-branch

## Config/Setup


git config --global --get user.name
git config --local --get user.name

Git will still use your global computer name and email for commits unless you change it. Inside your specific project folder, set the local identity so your commits map to the correct GitHub profile:
bash
# Inside your work project folder:
git config local user.name "Your Work Name"
git config local user.email "work_email@example.com"

# Inside your personal project folder:
git config local user.name "Your Personal Name"
git config local user.email "personal_email@example.com"
Use code with caution.
To make sure your active configurations match, could you tell me:


git remote add origin git@github.com:username/repo-name.git