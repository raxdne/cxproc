// https://developer.mozilla.org/en/Core_JavaScript_1.5_Guide/Predefined_Core_Objects/Date_Object

today = new Date()
endYear = new Date(1995,11,31,23,59,59,999) // Set day and month
endYear.setFullYear(today.getFullYear()) // Set year to this year
msPerDay = 24 * 60 * 60 * 1000 // Number of milliseconds per day
daysLeft = (endYear.getTime() - today.getTime()) / msPerDay
daysLeft = Math.round(daysLeft) //returns days left in the year

print(daysLeft);