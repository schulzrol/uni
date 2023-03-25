Feature: Store search and checkout
  Scenario: Product search
    Given guest user G is on the store homepage
    When user G searches for "product name"

  Scenario: Search from the search bar
    Given a web browser is at the OpenCart store home page
    When the user searches "phone" using the search bar
    Then products related to "phone" are shown

  Scenario: Add product to cart
    Given a web browser is at OpenCart store product P page
    When user adds the product to cart
    Then the product P is added to the users cart

  Scenario: Available Product Buy
    Given product P in users cart
    And P is availaible in specified quantity
    When user goes to the checkout page
    And user fills in valid shipping and payment information
    And user confirms order
    Then user sees a success page about the order being placed 
    And user sees order in order history

  Scenario: Unvailable Product Buy
    Given product P in users cart
    And P is unavailaible in specified quantity
    When user tries to go to the checkout page
    Then user should see alert message mentioning product P is "not available" in desired quantity