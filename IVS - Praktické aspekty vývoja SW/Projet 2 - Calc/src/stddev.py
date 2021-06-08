
"""@brief Code for profilig of calculating standard deviation
   
   @author IVS-DREAM-TEAM
   
   @file stddev.py
"""



import cProfile
import lib.profiler as pf
import sys

def help():
    """
    Function for help if input is uncorrect
    """    
    print("Usage: python stddev.py <stdin")
    sys.exit(1)

def main():
    if not sys.stdin.isatty():
        pr = cProfile.Profile()
        pf.Profiler(sys.stdin,pr)
        sys.exit(0)
    else:
        help()

if __name__ == "__main__":
    main()
