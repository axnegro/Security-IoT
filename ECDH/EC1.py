from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PrivateKey
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PublicKey

#################################################################################################
#################################################################################################
#           DIFFIE-HELLMAN ALGORITHM ( KEY EXCHANGE ) BASED ON DISCRETE LOGARITHM               #
#                               Author : Nous ( 2021 )                                          #
#################################################################################################
#################################################################################################


#################################################################################################
# Alice : Send Public Information for Key Exchange Request to the server ( publish g, p and A ) #
#################################################################################################

# Generate a private key for use in the exchange.
Alice_private_key = X25519PrivateKey.generate()
Alice_public_key = Alice_private_key.public_key()

A_public_key  = Alice_public_key.public_bytes(encoding=serialization.Encoding.Raw,format=serialization.PublicFormat.Raw)
A_private_key = Alice_private_key.private_bytes(encoding=serialization.Encoding.Raw,format=serialization.PrivateFormat.Raw,encryption_algorithm=serialization.NoEncryption())

# Display Public and Private information from Alice
print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
print("Public Information from Alice and send to Bob : ")
print("------------------------------------------------")
print("\t - Public Key (A)   : \n", A_public_key.hex())
print("Private Information from Alice : ")
print("------------------------------------------------")
print("\t - Private Key (a)  : \n", A_private_key.hex())

#################################################################################################
# Bob : Receive Public Information from Alice and compute public and private information (b, B) #
#################################################################################################

Bob_private_key = X25519PrivateKey.generate() # a new instance of DHPrivateKey
Bob_public_key  = Bob_private_key.public_key()          # a new instance of DHPublicKey

B_public_key  = Bob_public_key.public_bytes(encoding=serialization.Encoding.Raw,format=serialization.PublicFormat.Raw)
B_private_key = Bob_private_key.private_bytes(encoding=serialization.Encoding.Raw,format=serialization.PrivateFormat.Raw,encryption_algorithm=serialization.NoEncryption())

# # Display Public and Private information from Alice
print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
print("Public Information from Bob and send to Alice : ")
print("------------------------------------------------")
print("\t - Public Key (B)   : \n", B_public_key.hex())
print("Private Information from Bob : ")
print("------------------------------------------------")
print("\t - Private Key (b)  : \n", B_private_key.hex())

#################################################################################################
# Alice and Bob : Compute Shared Key                                                            #
#################################################################################################

# Compute the shared secret for the key exchange
Alice_Shared_Key = Alice_private_key.exchange(X25519PublicKey.from_public_bytes(B_public_key))
Bob_Shared_Key   = Bob_private_key.exchange(X25519PublicKey.from_public_bytes(A_public_key))

# Display Shared Key
print("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%")
print("Shared Secret between Alice and Bob : ")
print("------------------------------------------------")
print("\t - Shared Key compute by Alice : ", Alice_Shared_Key.hex())
print("\t - Shared Key compute by Bob   : ", Bob_Shared_Key.hex())