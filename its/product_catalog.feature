Feature: Managing of product catalog
    Scenario: Add new product
        Given user A logged in as admin
        When user A is on Add New Product page
        And user A fills in mandatory product details
        And user A clicks on Save button
        Then the new product should be present in products catalog
    
    Scenario: Change product availability
        Given user A logged in as admin
        And product P with nonzero quantity present in product catalog
        And product P out of stock status is set to S
        When user A sets product P quantity to 0
        And user A clicks on Save button
        Then product P availability changes to S
        And product P quantity is 0
    
    

    