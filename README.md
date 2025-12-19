Help me out with the planetary positioning math in:
`src/planets.c`

# Solar System

View the relative positions of the planets on any day

This app lets you simulate the positions of the planets on a 2D plane. Many liberties are taken to position the planets on the low resolution of the watch but an attempt has been made to position them accurately with their orbits normalized to a 2D circle around the sun.

To navigate through time there is multi-press system:
1 Press then a long press, you can navigate up or down in days
2 Presses then a long press, you can navigate in weeks
3 Presses then a long press, you can navigate in months
4 Presses then a long press, you can navigate in years.

Getting the multi press gesture right takes some timing, but you have to see the time change each press for it to be considered registered, then hold after the correct time has passed once.

This was the first app I had ever tried to make for the Pebble back when it first launched but I was much less capable of the math required to make it as accurate as I was happy with. I don't think it serves any utility other than it makes me happy that I was able to get this far with it and it's fun to show friends "look this is where the planets sort of were on your birthday" and receive a muted response about it.

# Dependencies 
Dependencies are installed using a personal script to download from GitHub. They can all be found in the following repo:
- https://github.com/CometDog/pebble-libraries

This is a legacy app using legacy systems and may be annoying to self-build. I do not really have plans to upgrade it because it "just works" on my machine.
