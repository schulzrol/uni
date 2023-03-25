Feature: Customer account management
    Scenario: Registering account 
        Given a web browser at account registering page
        When user enters valid full name, password
        And user enters valid email not used by any other user
        And user agrees to Privacy Policy
        And user clicks Continue
        Then new customer account with specified details is created
        And user is signed up into the new customer account

    Scenario: Registering account - email already in use
        Given a web browser at account registering page
        When user enters valid full name, password
        And user enters the same email used by an already registered account
        And user agrees to Privacy Policy
        And user clicks Continue
        Then popup warning message notifies user that specified email is "already registered"
        And no new customer account is created
    Scen