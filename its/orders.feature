Feature: Order management
    Scenario: Account order history - new account
        Given user creates a new account
        And user logs in to this new account
        When user displays order history page
        Then no orders are present in order history list

    Scenario: Account order history
        Given user logged into an customer account
        When user confirms new order
        Then new order is displayed in order history