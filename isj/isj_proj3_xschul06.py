#!/usr/bin/env python3

# ukol za 2 body
def first_odd_or_even(numbers):
    """Returns 0 if there is the same number of even numbers and odd numbers
       in the input list of ints, or there are only odd or only even numbers.
       Returns the first odd number in the input list if the list has more even
       numbers.
       Returns the first even number in the input list if the list has more odd 
       numbers.

    >>> first_odd_or_even([2,4,2,3,6])
    3
    >>> first_odd_or_even([3,5,4])
    4
    >>> first_odd_or_even([2,4,3,5])
    0
    >>> first_odd_or_even([2,4])
    0
    >>> first_odd_or_even([3])
    0
    """
    
    def is_even(n):
        return (n%2 == 0)

    # +1 even, -1 odd
    parity = sum([1 if is_even(num) else -1 for num in numbers])
    if (parity == 0 or abs(parity) == len(numbers)):
        # Handles first RETURNS requirement
        return 0;
    
    if (parity > 0): # more even
        return [num for num in numbers if not is_even(num)][0] # return first odd
    else: # more odd
        return [num for num in numbers if is_even(num)][0] # return first even




# ukol za 3 body
def to_pilot_alpha(word):
    """Returns a list of pilot alpha codes corresponding to the input word

    >>> to_pilot_alpha('Smrz')
    ['Sierra', 'Mike', 'Romeo', 'Zulu']
    """

    pilot_alpha = ['Alfa', 'Bravo', 'Charlie', 'Delta', 'Echo', 'Foxtrot',
        'Golf', 'Hotel', 'India', 'Juliett', 'Kilo', 'Lima', 'Mike',
        'November', 'Oscar', 'Papa', 'Quebec', 'Romeo', 'Sierra', 'Tango',
        'Uniform', 'Victor', 'Whiskey', 'Xray', 'Yankee', 'Zulu']

    pilot_alpha_dict = {alpha[0].lower(): alpha for alpha in pilot_alpha}

    pilot_alpha_list = [pilot_alpha_dict[letter.lower()] for letter in word]
     
    return pilot_alpha_list


if __name__ == "__main__":
    import doctest
    doctest.testmod()
