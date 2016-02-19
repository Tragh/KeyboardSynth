from math import*

for x in range(0, 60, 2):
	Player.PushNote(Notes.C4,0.6,"FM2",0.3,x)
	Player.PushNote(Notes.C5,0.6,"FM2",0.2,x+0.5)
	Player.PushNote(Notes.C4,0.6,"FM2",0.3,x+1)
	Player.PushNote(Notes.E5,0.6,"FM2",0.2,x+1.5)
	
for x in range(4, 60, 2):
	y=x+0.2
	Player.PushNote(Notes.E3,0.3,"FM1",0.3,y)
	Player.PushNote(Notes.E4,0.3,"FM1",0.2,y+0.5)
	Player.PushNote(Notes.E3,0.3,"FM1",0.3,y+1)
	Player.PushNote(Notes.G4,0.3,"FM1",0.2,y+1.5)

for x in range(8,16,4):
	Player.PushNote(Notes.B5,1,"bell",0.5,x)
	Player.PushNote(Notes.Gs5,1,"bell",0.5,x+0.5)
	Player.PushNote(Notes.A5,1,"bell",0.5,x+1)
	Player.PushNote(Notes.Gs5,1,"bell",0.5,x+1.5)
	Player.PushNote(Notes.Gs5,1,"bell",0.5,x+2)
	Player.PushNote(Notes.F5,1,"bell",0.5,x+2.5)
	Player.PushNote(Notes.D5,1,"bell",0.8,x+3)
	
for x in range(16,40,4):
	Player.PushNote(Notes.B5,1,"bell",0.5,x)
	Player.PushNote(Notes.Gs5,1,"bell",0.2,x+0.5)
	Player.PushNote(Notes.Gs5,1,"bell",0.3,x+0.7)
	Player.PushNote(Notes.A5,1,"bell",0.5,x+1)
	Player.PushNote(Notes.Gs5,1,"bell",0.2,x+1.5)
	Player.PushNote(Notes.Gs5,1,"bell",0.3,x+1.7)
	Player.PushNote(Notes.Gs5,1,"bell",0.5,x+2)
	Player.PushNote(Notes.F5,1,"bell",0.2,x+2.5)
	Player.PushNote(Notes.F5,1,"bell",0.3,x+2.7)
	Player.PushNote(Notes.D5,1,"bell",0.8,x+3)

	
	
